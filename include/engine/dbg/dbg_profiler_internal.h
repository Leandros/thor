#ifndef DBG_PROFILER_INTERNAL_H
#define DBG_PROFILER_INTERNAL_H


/* ========================================================================= */
/* Configuration                                                             */
/* ========================================================================= */

/* Defines the maximum stack depth of the profiler. */
#define STACK_SIZE          1024

/* Defines the maximum amount of frames being able to hold simultaneously. */
#define NUM_FRAMES          60

/* Number of average samples per frame. Will be multiplied by \c NUM_FRAMES. */
#define NUM_SAMPLES         2048

/*
 * Size of the ring buffer caching the samples per frame.
 * Must be a multiple of 64k.
 */
#define BUFFER_SIZE         (0x10000 * 4)


/* ========================================================================= */
/* Templates                                                                 */
/* ========================================================================= */
struct dbg_sample {
    u64 start;
    u64 end;
    u32 length, length_us;
    char const *name;
    struct dbg_sample *parent;
    struct dbg_sample *first_child;
    struct dbg_sample *last_child;
    struct dbg_sample *next_sibling;
    struct dbg_sample *_next; /* required for flattening the tree. */
};

struct dbg_sample_frame {
    u64 start, end;
    u32 length, length_us;
    u32 frame, samples;
    struct dbg_sample *root;
    struct dbg_sample *current;

    struct dbg_sample_frame *prev;
    struct dbg_sample_frame *next;
};

#define T struct dbg_sample *
#define DEQUE_NAME deque_sample
#define WITH_STRUCT
#include <tpl/tpl_deque.h>


/* ========================================================================= */
/* Internal State                                                            */
/* ========================================================================= */
struct dbg_profiler_state {
    /* Settings */
    u32 enabled;
    u16 will_disable, will_enable;
    /* 4 byte spill. */

    /* Call tree stack */
    struct deque_sample cpu_stack[MAX_THREADS];
    /* Root frame of every thread. */
    struct dbg_sample_frame *frame_root[MAX_THREADS];
    /* Current frame of every thread. */
    struct dbg_sample_frame *frame_current[MAX_THREADS];

    /* Memory for frames. */
    struct mem_alloc_pool *frame_pool[MAX_THREADS];
    struct mem_alloc_pool *sample_pool[MAX_THREADS];

    /* String buffer for names. */
    char *str_buf;
    u32 str_ptr, str_siz;

    /* TLS IDs */
    unsigned long tls_thread_id;

    /* Misc */
    u32 thread_ptr;
    u32 current_frame;
    u64 freq;

    /* Synchronization */
    sys_spinlock lock_stack;
};

#endif /* DBG_PROFILER_INTERNAL_H */

