#define WITH_GFX
#include <sys/sys_d3d11.h>
#include <engine.h>
#include <tpl/tpl_sort.h>
#include <mem/mem_alloc.h>


/* ========================================================================= */
/* Internal Config                                                           */
#define QUEUE_SIZE          1024
#define CMD_POOL_SIZE       1024
#define DRAW_POOL_SIZE      2048


/* ========================================================================= */
/* Private Functions: */
static struct gfx_cmd *retain_all(struct gfx_cmd *cmd);
static struct gfx_cmd *release_all(struct gfx_cmd *cmd);
static u64 make_key(struct gfx_cmd *cmd, u8 pass, u16 depth);
static int cmp_draw(struct gfx_cmd_draw *lhs, struct gfx_cmd_draw *rhs);

/* ========================================================================= */
/* Structures                                                                */
struct gfx_cmd_draw {
    u64 key;
    struct gfx_cmd *cmd;
    struct gfx_draw *draw;
};

/* ========================================================================= */
/* Vectors                                                                   */
#define T struct gfx_cmd_draw
#define VEC_NAME vec_queue
#define WITH_IMPL
#include <tpl/tpl_vector.h>

/* ========================================================================= */
/* Internal State                                                            */
struct gfx_cmd_state {
    /* Internal */
    struct gfx_resources *res;
    u32 cmd_id;
    u32 draw_id;

    /* Memory */
    void *cmd_mem;
    void *draw_mem;

    /* Containers */
    struct mem_alloc_pool *cmd_pool;
    struct mem_alloc_pool *draw_pool;
    struct vec_queue queue;

    /* Spinlocks */
    sys_spinlock queue_lock;
    sys_spinlock cmd_pool_lock;
    sys_spinlock draw_pool_lock;

    /* Render Defaults */
    u8 rasterizer;
    u8 blend_mode;
    u8 depth_stencil;
    u8 program;
    u16 input_assembler;
};

TPL_SORT_DECLARE(queue, struct gfx_cmd_draw)
TPL_SORT_DEFINE(queue, struct gfx_cmd_draw)


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
struct gfx_cmd_state *
gfx_cmd_init(void)
{
    usize size;
    struct gfx_cmd_state *q;
    struct gfx_state *s = g_state->gfx_state;

    q = mem_malloc(sizeof(struct gfx_cmd_state));
    q->res = gfx_d3d11_state_init();
    dbg_err(q->res == NULL, "initializing d3d11 resources", goto e0);
    sys_spinlock_init(&q->queue_lock);
    sys_spinlock_init(&q->cmd_pool_lock);
    sys_spinlock_init(&q->draw_pool_lock);

    q->cmd_id = 0;
    q->draw_id = 0;
    vec_queue_alloc(&q->queue, QUEUE_SIZE);

    /* Command pool */
    size = mem_pool_memreq(CMD_POOL_SIZE + 1, sizeof(struct gfx_cmd));
    q->cmd_mem = mem_malloc(size);
    q->cmd_pool = mem_pool_create(q->cmd_mem, size, sizeof(struct gfx_cmd));
    dbg_err(q->cmd_pool == NULL, "creating memory pool", goto e1);

    /* Draw pool */
    size = mem_pool_memreq(DRAW_POOL_SIZE + 1, sizeof(struct gfx_draw));
    q->draw_mem = mem_malloc(size);
    q->draw_pool = mem_pool_create(q->draw_mem, size, sizeof(struct gfx_draw));
    dbg_err(q->draw_pool == NULL, "creating memory pool", goto e2);

    /* Render Defaults: */
    /* TODO: */
    return q;

e2: mem_free(q->draw_mem);
e1: mem_free(q->cmd_mem);
    mem_free(vec_queue_mem(&q->queue));
e0: mem_free(q);
    return NULL;
}

void
gfx_cmd_destroy(struct gfx_cmd_state *q)
{
    mem_free(q->cmd_mem);
    mem_free(q->draw_mem);
    mem_free(vec_queue_mem(&q->queue));
    gfx_d3d11_state_quit(q->res);
    mem_free(q);
}

INLINE void
sort_draw_queue(struct vec_queue *q)
{
    profile_function_start();
    sort(queue, vec_queue_first(q), vec_queue_last(q) + 1, &cmp_draw);
    profile_function_end();
}

void
gfx_cmd_render(void)
{
    usize i, n;
    struct gfx_cmd_draw *obj;
    struct gfx_device *s = g_state->gfx_state->device;
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;
    profile_function_start();

    n = vec_queue_length(&q->queue);
    if (n > 0) {
        sort_draw_queue(&q->queue);

        for (i = 0; i < n; ++i) {
            obj = vec_queue_get(&q->queue, (u32)i);
            gfx_d3d11_draw(obj->cmd, obj->draw);
            release_all(obj->cmd);
        }
    }

    /* Reset */
    q->cmd_id = 0;
    q->draw_id = 0;
    s->current_cmd = 0xFFFFFFFF;
    memset(&s->state, 0xFF, sizeof(struct gfx_cmd));
    vec_queue_clear(&q->queue);
    mem_pool_reset(q->cmd_pool);
    mem_pool_reset(q->draw_pool);
    profile_function_end();
}


/* ========================================================================= */
/* Command Functions                                                         */
/* ========================================================================= */
INLINE struct gfx_cmd *
make_cmd(void)
{
    struct gfx_cmd *cmd;
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;

    sys_spinlock_lock(&q->cmd_pool_lock);
    cmd = mem_pool_alloc(q->cmd_pool, 0);
    sys_spinlock_unlock(&q->cmd_pool_lock);

    dbg_assert(cmd != NULL, "command buffer pool exhausted");
    return cmd;
}

struct gfx_cmd *
gfx_cmd_get(void)
{
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;
    struct gfx_cmd *cmd = make_cmd();
    memset(cmd, 0xFF, sizeof(struct gfx_cmd));
    cmd->__id = q->cmd_id++;
    return cmd;
}

struct gfx_cmd *
gfx_cmd_cpy(struct gfx_cmd *old)
{
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;
    struct gfx_cmd *new = make_cmd();
    memcpy(new, old, sizeof(struct gfx_cmd));
    new->__id = q->cmd_id++;
    return new;
}

void
gfx_cmd_reset(struct gfx_cmd *cmd)
{
    memset(cmd, 0xFF, sizeof(struct gfx_cmd));
    cmd->__id = -1;
}

static void
compile(struct gfx_cmd *out, struct gfx_cmd *arr, usize len)
{
    u16 id;
    usize i, j;

    id = out->__id;
    memset(out, 0xFF, sizeof(struct gfx_cmd));
    out->__id = id;

    for (i = 0; i < len; ++i) {
        if (ID_INVALID(out->rasterizer))
            out->rasterizer = arr[i].rasterizer;
        if (ID_INVALID(out->blend_mode))
            out->blend_mode = arr[i].blend_mode;
        if (ID_INVALID(out->depth_stencil))
            out->depth_stencil = arr[i].depth_stencil;
        if (ID_INVALID(out->program))
            out->program = arr[i].program;

        /* Arrays are evaluated on a per-slot basis. */
        for (j = 0; j < 4; ++j)
            if (ID_INVALID(out->uavs[j]))
                out->uavs[j] = arr[i].uavs[j];
        for (j = 0; j < 4; ++j)
            if (ID_INVALID(out->cbuffers[j]))
                out->cbuffers[j] = arr[i].cbuffers[j];
        for (j = 0; j < 8; ++j)
            if (ID_INVALID(out->samplers[j]))
                out->samplers[j] = arr[i].samplers[j];
        for (j = 0; j < 8; ++j)
            if (ID_INVALID(out->resources[j]))
                out->resources[j] = arr[i].resources[j];

        if (ID_INVALID(out->input_assembler))
            out->input_assembler = arr[i].input_assembler;
    }

    /* TODO: Apply Defaults */
}

struct gfx_cmd *
gfx_cmd_compile(struct gfx_cmd *arr, usize len)
{
    struct gfx_cmd *out = gfx_cmd_get();

    /* Assemble final command */
    compile(out, arr, len);

    return out;
}


/* ========================================================================= */
/* Draw Functions                                                            */
/* ========================================================================= */
INLINE struct gfx_draw *
make_draw(void)
{
    struct gfx_draw *draw;
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;

    sys_spinlock_lock(&q->draw_pool_lock);
    draw = mem_pool_alloc(q->draw_pool, 0);
    sys_spinlock_unlock(&q->draw_pool_lock);

    dbg_assert(draw != NULL, "draw buffer pool exhausted");
    return draw;
}

struct gfx_draw *
gfx_draw_get(void)
{
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;
    struct gfx_draw *draw = make_draw();
    memset(draw, 0x0, sizeof(struct gfx_draw));
    draw->__id = q->draw_id++;
    return draw;
}

struct gfx_draw *
gfx_draw_cpy(struct gfx_draw *old)
{
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;
    struct gfx_draw *new = make_draw();
    memcpy(new, old, sizeof(struct gfx_draw));
    new->__id = q->draw_id++;
    return new;
}


/* ========================================================================= */
/* Render Queue Functions                                                    */
/* ========================================================================= */
void
gfx_cmd_queue(struct gfx_cmd *cmd, struct gfx_draw *draw)
{
    struct gfx_cmd_draw obj;
    struct gfx_cmd_state *q = g_state->gfx_state->rqueue;

    dbg_assert(ID_VALID(cmd->__id),
            "command buffer id is invalid, likely stack allocated");
    dbg_assert(ID_VALID(draw->__id),
            "draw buffer id is invalid, likely stack allocated");

    obj.key = make_key(cmd, 0, draw->depth);
    obj.cmd = retain_all(cmd);
    obj.draw = draw;

    /* Submit to queue. */
    sys_spinlock_lock(&q->queue_lock);
    vec_queue_push(&q->queue, &obj);
    sys_spinlock_unlock(&q->queue_lock);
}


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
static struct gfx_cmd *
retain_all(struct gfx_cmd *cmd)
{
    usize i;

    if (ID_VALID(cmd->rasterizer))
        gfx_rasterizer_retain(cmd->rasterizer);
    if (ID_VALID(cmd->blend_mode))
        gfx_blend_retain(cmd->blend_mode);
    if (ID_VALID(cmd->depth_stencil))
        gfx_depthstencil_retain(cmd->depth_stencil);
    if (ID_VALID(cmd->program))
        gfx_program_retain(cmd->program);

    /* TODO: Implement UAVs */
    /* for (i = 0; i < 4; ++i) */
    /*     if (ID_VALID(cmd->uavs[i])) */

    for (i = 0; i < 4; ++i)
        if (ID_VALID(cmd->cbuffers[i]))
            gfx_buffer_retain(cmd->cbuffers[i]);

    for (i = 0; i < 8; ++i)
        if (ID_VALID(cmd->samplers[i]))
            gfx_sampler_retain(cmd->samplers[i]);

    for (i = 0; i < 8; ++i)
        if (ID_VALID(cmd->resources[i]))
            gfx_resourcelist_retain(cmd->resources[i]);

    if (ID_VALID(cmd->input_assembler))
        gfx_iastage_retain(cmd->input_assembler);

    return cmd;
}

static struct gfx_cmd *
release_all(struct gfx_cmd *cmd)
{
    usize i;

    gfx_rasterizer_release(cmd->rasterizer);
    gfx_blend_release(cmd->blend_mode);
    gfx_depthstencil_release(cmd->depth_stencil);
    gfx_program_release(cmd->program);

    /* TODO: Implement UAVs */
    /* for (i = 0; i < 4; ++i) */

    for (i = 0; i < 4; ++i)
        gfx_buffer_release(cmd->cbuffers[i]);
    for (i = 0; i < 8; ++i)
        gfx_sampler_release(cmd->samplers[i]);
    for (i = 0; i < 8; ++i)
        gfx_resourcelist_release(cmd->resources[i]);
    gfx_iastage_release(cmd->input_assembler);

    return cmd;
}

/* TODO: Expose renderpasses */
#define PASS_PREDRAW    0x0
#define PASS_FULLSCREEN 0x1
#define POST_DRAW       0x4


static u64
make_key(struct gfx_cmd *cmd, u8 pass, u16 depth)
{
    u64 key;
    u16 material;

    /* TODO: Add material. */
    material = 0;

    /*
     * Depth is *reversed* to draw from the back-to-front, not from
     * front-to-back.
     */
    depth = 0xFFFF - depth;

    /*
     * The key is constructed from the various resource IDs, to minimize state
     * changes in the renderer.
     *
     * Layout:
     * 64 - 61: Renderpass (3)
     * 61 - 56: Unused (5)
     * 56 - 52: Blend (4)
     * 52 - 48: Depth (4)
     * 48 - 40: Program (8)
     * 40 - 32: Input Assembler (8)
     * 32 - 16: Material (16)
     * 16 -  1: Depth (16)
     *
     * Changes: 61-56 was formerly used to sort by rasterizer. This breaks
     * ImGui rendering. Rasterization state sorting removed.
     */

    /* Assert that mentioned constraints are true! */
    dbg_assert(pass < 8, "to many bits used");
    /* dbg_assert(cmd->rasterizer < 32, "to many bits used"); */
    dbg_assert(cmd->blend_mode < 16, "to many bits used");
    dbg_assert(cmd->depth_stencil < 16, "to many bits used");
    dbg_assert(cmd->program < 256, "to many bits used");
    dbg_assert(cmd->input_assembler < 256, "to many bits used");
    dbg_assert(material < 65536, "to many bits used");
    dbg_assert(depth < 65536, "to many bits used");

    /* Construct the key. */
    key =
        (((u64)pass & 0x7) << 61) |
        /* (((u64)cmd->rasterizer & 0x1F) << 56) | */
        (((u64)0 & 0x1F) << 56) |
        (((u64)cmd->blend_mode & 0xF) << 52) |
        (((u64)cmd->depth_stencil & 0xF) << 48) |
        (((u64)cmd->program & 0xFF) << 40) |
        (((u64)cmd->input_assembler & 0xFF) << 32) |
        (((u64)material & 0xFFFF) << 16) |
        (((u64)depth & 0xFFFF) << 0);

    return key;
}

static int
cmp_draw(struct gfx_cmd_draw *lhs, struct gfx_cmd_draw *rhs)
{
    return lhs->key > rhs->key;
}

