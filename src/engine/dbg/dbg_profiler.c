#include <engine.h>
#include <mem/mem_alloc.h>
#include <mem/mem_magicringbuffer.h>
#include <sys/sys_threads.h>
#include <sys/sys_timer.h>
#include <dbg/dbg_profiler.h>
#include <dbg/dbg_profiler_internal.h>
#include <ext/xxhash.h>

/* ========================================================================= */
/* Internal Function                                                         */
static struct dbg_sample *flatten_sample_tree(struct dbg_sample *root);
static void free_sample_frame(struct dbg_sample_frame *frame, usize cpu);

/* ========================================================================= */
/* Templates                                                                 */
#define T struct dbg_sample *
#define DEQUE_NAME deque_sample
#define NO_STRUCT
#define WITH_IMPL
#include <tpl/tpl_deque.h>

/* ========================================================================= */
/* Functions                                                                 */

int
dbg_profiler_init(void)
{
    usize i, n, p, size;
    char *mem;
    struct dbg_profiler_state *s;
    struct dbg_sample_frame *frame;

    s = mem_malloc(sizeof(struct dbg_profiler_state));
    g_state->profiler = s;

    s->str_buf = mem_malloc(BUFFER_SIZE);
    s->str_ptr = 0, s->str_siz = 0;

    s->thread_ptr = 0;
    s->current_frame = 0;
    s->freq = perf_freq();
    sys_spinlock_init(&s->lock_stack);
    s->enabled = 1;
    s->will_disable = 0;

    /* Setup thread locals. */
    s->tls_thread_id = TLS_ALLOC();
    dbg_err(TLS_FAILED(s->tls_thread_id), "creating TLS thread id", goto e0);

    /* Setup CPU stacks. */
    n = (usize)sys_num_cpu();
    size = deque_sample_memreq(STACK_SIZE);
    mem = mem_malloc(size * n);
    for (i = 0, p = 0; i < n; ++i)
        deque_sample_init(&s->cpu_stack[i], mem + p, STACK_SIZE),
            p += size;

    /* Setup the frame pool alloctor. */
    size = mem_pool_memreq(NUM_FRAMES, sizeof(struct dbg_sample_frame));
    mem = mem_malloc(size * n);
    for (i = 0, p = 0; i < n; ++i)
        s->frame_pool[i] = mem_pool_create(
                mem + p, size, sizeof(struct dbg_sample_frame)),
            p += size;

    /* Setup the sample pool alloctor. */
    size = mem_pool_memreq(NUM_SAMPLES * NUM_FRAMES, sizeof(struct dbg_sample));
    mem = mem_malloc(size * n);
    for (i = 0, p = 0; i < n; ++i)
        s->sample_pool[i] = mem_pool_create(
                mem + p, size, sizeof(struct dbg_sample)),
            p += size;

    /* Setup the initial root frames. */
    for (i = 0; i < n; ++i) {
        frame = mem_pool_alloc(s->frame_pool[i], MEM_ALLOC_ZERO);
        s->frame_root[i] = frame;
        s->frame_current[i] = frame;
    }

    return 0;

e0: mem_free(s);
    g_state->profiler = NULL;
    return 1;
}

void
dbg_profiler_init_thread(void)
{
    struct dbg_profiler_state *s = g_state->profiler;
    dbg_assert(TLS_GET(s->tls_thread_id) == NULL, "thread already registered");

    sys_spinlock_lock(&s->lock_stack);
    {
        s->thread_ptr += 1;
        TLS_SET(s->tls_thread_id, (void *)(u64)s->thread_ptr);
    }
    sys_spinlock_unlock(&s->lock_stack);
}

void
dbg_profiler_quit(void)
{
    struct dbg_profiler_state *s = g_state->profiler;

    TLS_FREE(s->thread_ptr);
    mem_free(deque_sample_mem(&s->cpu_stack[0]));
    mem_pool_destroy(s->frame_pool[0]);
    mem_pool_destroy(s->sample_pool[0]);
    mem_free(s->frame_pool[0]);
    mem_free(s->sample_pool[0]);
}

void
dbg_profiler_enabled(int enabled)
{
    g_state->profiler->enabled = enabled;
    g_state->profiler->will_disable = !enabled;
    g_state->profiler->will_enable = (u16)enabled;
}


/* ========================================================================= */
/* Profiler                                                                  */
void
dbg_profiler_frame_start(void)
{
    u32 i, n;
    struct dbg_sample_frame *curr, *new_root;
    struct dbg_profiler_state *s = g_state->profiler;
    if (!s->enabled)
        return;
    s->will_enable = 0;

    /* Prepare the current frame. */
    for (i = 0, n = s->thread_ptr; i < n; ++i) {
        curr = s->frame_current[i];
        curr->frame = s->current_frame;
        curr->root = mem_pool_alloc(s->sample_pool[i], MEM_ALLOC_ZERO);

        /* Free last frame, get new free samples. */
        if (curr->root == NULL) {
            new_root = s->frame_root[i]->next;
            free_sample_frame(s->frame_root[i], i);
            s->frame_root[i] = new_root;
            curr->root = mem_pool_alloc(s->sample_pool[i], MEM_ALLOC_ZERO);
            dbg_assert(curr->root != NULL, "no free samples");
        }

        curr->current = curr->root;
        curr->start = perf_ticks();
    }
}

void
dbg_profiler_frame_end(void)
{
    u32 i, n;
    u64 elapsed;
    struct dbg_sample_frame *prev, *next, *new_root;
    struct dbg_profiler_state *s = g_state->profiler;
    if (!s->enabled && !s->will_disable)
        return;
    if (s->will_enable)
        return;
    s->will_disable = 0;

    /* Advance the frame. */
    for (i = 0, n = s->thread_ptr; i < n; ++i) {
        prev = s->frame_current[i];
        prev->end = perf_ticks();
        next = mem_pool_alloc(s->frame_pool[i], MEM_ALLOC_ZERO);

        /* Remove the current root frame, replace by next frame to make space. */
        if (next == NULL) {
            /* TODO: Consider removing the last 120 or so frames, instead of one. */
            new_root = s->frame_root[i]->next;
            free_sample_frame(s->frame_root[i], i);
            s->frame_root[i] = new_root;
            next = mem_pool_alloc(s->frame_pool[i], MEM_ALLOC_ZERO);
            dbg_assert(next != NULL, "no free frames");
        }

        elapsed = prev->end - prev->start;
        prev->length = (u32)elapsed;
        elapsed *= PERF_TO_MICROSECONDS;
        elapsed /= s->freq;
        prev->length_us = (u32)elapsed;
        prev->next = next;
        next->prev = prev;
        s->frame_current[i] = next;
    }


    s->current_frame += 1;
}

void
dbg_profiler_begin(char const *function, char const *name)
{
    struct dbg_profiler_state *s = g_state->profiler;
    if (!s->enabled || s->will_disable || s->will_enable)
        return;

    {
        usize cpu = (u64)TLS_GET(s->tls_thread_id) - 1;
        struct deque_sample *q = &s->cpu_stack[cpu];
        struct dbg_sample_frame *frame = s->frame_current[cpu];
        struct mem_alloc_pool *sample_pool = s->sample_pool[cpu];
        struct dbg_sample *sample, *parent;

        sample = mem_pool_alloc(sample_pool, MEM_ALLOC_ZERO);
        if (sample == NULL) {
            struct dbg_sample_frame *new_root = s->frame_root[cpu]->next;
            free_sample_frame(s->frame_root[cpu], cpu);
            s->frame_root[cpu] = new_root;
            sample = mem_pool_alloc(sample_pool, MEM_ALLOC_ZERO);
            dbg_assert(sample != NULL, "no free samples");
        }

        sample->name = function;
        sample->parent = frame->current;

        parent = frame->current;
        /* TODO: Aggregate samples with the same name? */
        if (parent->first_child == NULL) {
            parent->first_child = sample;
            parent->last_child = sample;
        } else {
            parent->last_child->next_sibling = sample;
            parent->last_child = sample;
        }

        frame->current = sample;

        /* Start timer, push it to stack. */
        sample->start = perf_ticks();
        deque_sample_push(q, &sample);
    }
}

void
dbg_profiler_end(void)
{
    struct dbg_profiler_state *s = g_state->profiler;
    if (!s->enabled || s->will_disable || s->will_enable)
        return;

    {
        u64 elapsed, end = perf_ticks();
        usize cpu = (u64)TLS_GET(s->tls_thread_id) - 1;
        struct deque_sample *q = &s->cpu_stack[cpu];
        struct dbg_sample_frame *frame = s->frame_current[cpu];
        struct dbg_sample *sample;

        sample = *deque_sample_pop(q);
        elapsed = end - sample->start;
        sample->length = (u32)elapsed;
        elapsed *= PERF_TO_MICROSECONDS;
        elapsed /= s->freq;

        sample->end = end;
        sample->length_us = (u32)elapsed;

        frame->current = sample->parent;
        frame->samples += 1;
    }
}

void
gfx_profiler_begin(char const *name)
{
}

void
gfx_profiler_end(void)
{
}


/* ========================================================================= */
/* Internal Function                                                         */
static struct dbg_sample *
flatten_sample_tree(struct dbg_sample *root)
{
    struct dbg_sample *child, *curr, *last;

    curr = root;
    root->_next = root->first_child;
    for (child = root->first_child; child != NULL; child = child->next_sibling) {
        last = flatten_sample_tree(child);
        last->_next = child->next_sibling;
        curr = last;
    }

    root->first_child = NULL;
    root->last_child = NULL;

    return curr;
}

static void
free_sample_frame(struct dbg_sample_frame *frame, usize cpu)
{
    struct dbg_sample *next;
    struct dbg_profiler_state *s = g_state->profiler;
    dbg_assert(frame->root != NULL, "root may not be NULL");

    flatten_sample_tree(frame->root);
    for (next = frame->root->_next; next != NULL; next = next->_next)
        mem_pool_free(s->sample_pool[cpu], next);
    mem_pool_free(s->frame_pool[cpu], frame);
}

