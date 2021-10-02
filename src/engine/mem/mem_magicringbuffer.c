#include <engine.h>
#include <mem/mem_magicringbuffer.h>


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
static void *
determine_viable_addr(usize size)
{
    void *ptr;

    ptr = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    if (!ptr)
        return NULL;

    VirtualFree(ptr, 0, MEM_RELEASE);
    return ptr;
}

static void *
alloc_at(struct mem_mrb *ptr, usize size, void *base)
{
    void *ptr0, *ptr1;
    usize alloc_size = size * 2;

    ptr->handle = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            (u64)alloc_size >> 32,
            alloc_size & 0xFFFFFFFFu,
            NULL);
    if (!ptr->handle)
        return NULL;

    ptr0 = (void *)MapViewOfFileEx(
        ptr->handle, FILE_MAP_ALL_ACCESS, 0, 0, size, base);
    if (!ptr0)
        goto e0;

    ptr1 = (void *)MapViewOfFileEx(
        ptr->handle, FILE_MAP_ALL_ACCESS, 0, 0, size, (u8 *)base + size);
    if (!ptr1)
        goto e1;

    ptr->size = size;
    return ptr0;

e1: UnmapViewOfFile(ptr0);
e0: CloseHandle(ptr->handle);
    ptr->handle = NULL;
    return NULL;
}


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */
int
mem_mrb_init(struct mem_mrb *buf, usize size, void *base_addr)
{
    int num_retries = 0;
    buf->baseptr = NULL;
    buf->head = 0;
    buf->tail = 0;

    dbg_assert(size % 0x10000 == 0, "not a multiple of 64k");

    if (base_addr) {
        while (!buf->baseptr) {
            buf->baseptr = alloc_at(buf, size,
                    ((char *)BASE_ADDR) + ((size * 2) * num_retries));

            if (++num_retries > RETRY_COUNT)
                break;
        }
    } else {
        while (!buf->baseptr) {
            base_addr = determine_viable_addr(size * 2);
            if (!base_addr)
                buf->baseptr = alloc_at(buf, size, base_addr);

            if (++num_retries > RETRY_COUNT)
                break;
        }
    }

    return buf->baseptr == NULL;
}

void
mem_mrb_destroy(struct mem_mrb *buf)
{
    UnmapViewOfFile(buf->baseptr);
    UnmapViewOfFile(buf->baseptr + buf->size);
    CloseHandle(buf->handle);
}

usize
mem_mrb_wspace(struct mem_mrb *buf)
{
    if (buf->head > buf->tail)
        return buf->size - (buf->head - buf->tail);
    if (buf->head < buf->tail)
        return buf->tail - buf->head;
    return buf->size;
}

usize
mem_mrb_rspace(struct mem_mrb *buf)
{
    if (buf->head > buf->tail)
        return buf->head - buf->tail;
    if (buf->head < buf->tail)
        return buf->size - (buf->tail - buf->head);
    return 0;
}

usize
mem_mrb_write(struct mem_mrb *buf, char const *data, usize size)
{
    usize available;

    /* Never write more than space available. */
    available = mem_mrb_wspace(buf);
    if (size > available)
        size = available - 1;

    memcpy(buf->baseptr + buf->head, data, size);
    buf->head += size;

    /* Wrap around, if the end is reached. */
    if (buf->head > buf->size)
        buf->head = buf->head - buf->size;

    return size;
}

void
mem_mrb_write_noadv(struct mem_mrb *buf, char const *data, usize size)
{
    memcpy(buf->baseptr + buf->head, data, size);
}

char const *
mem_mrb_read(struct mem_mrb *buf, usize size, usize *read)
{
    usize available;
    char const *ptr;

    available = mem_mrb_rspace(buf);
    if (size > available)
        size = available - 1;

    ptr = buf->baseptr + buf->tail;
    buf->tail += size;

    /* If the end is reached, wrap around. */
    if (buf->tail > buf->size)
        buf->tail = buf->tail - buf->size;

    if (read)
        *read = size;

    return ptr;
}

char const *
mem_mrb_read_noadv(struct mem_mrb *buf, usize size, usize *read)
{
    usize available;
    char const *ptr;

    available = mem_mrb_rspace(buf);
    if (size > available)
        size = available - 1;

    ptr = buf->baseptr + buf->tail;

    if (read)
        *read = size;

    return ptr;
}

