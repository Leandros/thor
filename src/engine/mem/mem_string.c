#include <engine.h>
#include <libc/string.h>
#include <mem/mem_string.h>
#include <mem/mem_alloc.h>

/* Structures / typedefs */
struct mem_lstr {
    char *data;
    size_t len;
    size_t size;
    char buf[8];
};

struct mem_sstr {
    char data[sizeof(struct mem_lstr) - 1];
    unsigned char len;
};

union mem_str_union {
    struct mem_lstr l;
    struct mem_sstr s;
};


/* internal macros */
#define SSTR_SIZE (sizeof(struct mem_lstr) - 2)
#define SSTRUCT_SIZE (sizeof(union mem_str_union))

/* yes, horrible preprocessor hacks */
/* fuck, I have to fix these */
#define sstr (&(str->s))
#define lstr (&(str->l))

#define str_MALLOC  mem_malloc
#define str_FREE    mem_free

/* Internal functions */
INLINE int mem_str_is_long(mem_string *str);
INLINE void mem_str_set_long(mem_string *str);
INLINE void mem_str_set_slen(mem_string *str, uchar len);
INLINE uchar mem_str_get_slen(mem_string *str);
INLINE char *mem_str_data(mem_string *str);


mem_string *
mem_str_init(char const *str, size_t len)
{
    union mem_str_union *s;
    s = (union mem_str_union *)str_MALLOC(SSTRUCT_SIZE);
    if (s == NULL) return NULL;

    if (str != NULL) {
        if (len > SSTR_SIZE) {
            s->l.len = len;
            s->l.size = len + 1;
            s->l.data = (char*)str_MALLOC(len + 1);
            if (s->l.data == NULL) { str_FREE(s); return NULL; }
            memcpy(s->l.data, str, len);
            s->l.data[len] = '\0';
            mem_str_set_long(s);
        } else {
            memcpy(s->s.data, str, len);
            s->s.data[len] = '\0';
            mem_str_set_slen(s, (uchar)len);
        }
    } else {
        /* will zero everything, and make it a short string */
        memset(s, 0, SSTRUCT_SIZE);
    }

    return s;
}

void
mem_str_free(mem_string *str)
{
    if (str == NULL) return;
    if (mem_str_is_long(str)) {
        str_FREE(lstr->data);
    }

    str_FREE(str);
}

char *
mem_str_str(mem_string *str)
{
    return mem_str_data(str);
}

void
mem_str_set(mem_string *str, char const *s)
{
    char* tmp;
    size_t len;
    dbg_assert(s != NULL, "String must not be NULL");

    len = strlen(s);
    if (!mem_str_is_long(str) && len <= SSTR_SIZE) {
        memcpy(sstr->data, s, len);
        sstr->data[len] = '\0';
        mem_str_set_slen(str, (uchar)len);
    } else {
        if (!mem_str_is_long(str) || (len + 1) > lstr->size) {
            tmp = (char*)str_MALLOC(len + 1);
            memcpy(tmp, s, len);
            tmp[len] = '\0';

            /* do not attempt to free the data if isn't a long string */
            /* there will be rubbish in this variable */
            if (mem_str_is_long(str))
                str_FREE(lstr->data);

            lstr->data = tmp;
            lstr->len = len;
            lstr->size = len + 1;
            mem_str_set_long(str);
        } else {
            memcpy(lstr->data, s, len);
            lstr->data[len] = '\0';
            lstr->len = len;
        }
    }
}

char
mem_str_get(mem_string *str, size_t i)
{
    char c = 0;
    if (mem_str_is_long(str)) {
        if (i < lstr->len)
            c = lstr->data[i];
    } else {
        if (i < mem_str_get_slen(str))
            c = sstr->data[i];
    }

    return c;
}

void
mem_str_clear(mem_string *str)
{
    if (mem_str_is_long(str)) {
        lstr->len = 0;
    } else {
        mem_str_set_slen(str, 0);
    }
}

mem_string *
mem_str_prepend(mem_string *str, char const *s)
{
    char* tmp;
    size_t len;
    dbg_assert(s != NULL, "String must not be NULL");

    len = strlen(s);
    if (!mem_str_is_long(str) && (mem_str_get_slen(str) + len + 1) <= SSTR_SIZE) {
        uchar slen = mem_str_get_slen(str);
        memmove(sstr->data + len, sstr->data, slen);
        memcpy(sstr->data, s, len);
        sstr->data[len + slen] = '\0';
        mem_str_set_slen(str, (uchar)(slen + len));
    } else {
        if (!mem_str_is_long(str) || (lstr->len + len + 1) > lstr->size) {
            size_t llen = mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
            size_t size = (llen + len + 1) * MEM_STRING_RESIZE;
            char *data = mem_str_data(str);
            tmp = (char*)str_MALLOC(size);
            memcpy(tmp, s, len);
            memcpy(tmp + len, data, llen);
            tmp[len + llen] = '\0';

            if (mem_str_is_long(str))
                str_FREE(lstr->data);

            lstr->data = tmp;
            lstr->len = llen + len;
            lstr->size = size;
            mem_str_set_long(str);
        } else {
            memmove(lstr->data + len, lstr->data, lstr->len);
            memcpy(lstr->data, s, len);
            lstr->data[len + lstr->len] = '\0';
            lstr->len = lstr->len + len;
        }
    }

    return str;
}

mem_string *
mem_str_append(mem_string *str, char const *s)
{
    char *tmp;
    size_t len, size;
    dbg_assert(s != NULL, "String must not be NULL");

    len = strlen(s);
    if (!mem_str_is_long(str) && (mem_str_get_slen(str) + len + 1) <= SSTR_SIZE) {
        uchar slen = mem_str_get_slen(str);
        memcpy(sstr->data + slen, s, len);
        sstr->data[slen + len] = '\0';
        mem_str_set_slen(str, (uchar)(slen + len));
    } else {
        if (!mem_str_is_long(str) || (lstr->len + len + 1) > lstr->size) {
            size_t llen = mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
            char *data = mem_str_data(str);
            size = (llen + len + 1) * MEM_STRING_RESIZE;
            tmp = (char*)str_MALLOC(size);
            memcpy(tmp, data, llen);
            memcpy(tmp + llen, s, len);
            tmp[llen + len] = '\0';

            if (mem_str_is_long(str))
                str_FREE(lstr->data);

            lstr->data = tmp;
            lstr->len = llen + len;
            lstr->size = size;
            mem_str_set_long(str);
        } else {
            memcpy(lstr->data + lstr->len, s, len);
            lstr->data[lstr->len + len] = '\0';
            lstr->len = lstr->len + len;
        }
    }

    return str;
}

int
mem_str_push(mem_string *str, char c)
{
    char *tmp;
    int islong;

    islong = mem_str_is_long(str);
    if (!islong && ((size_t)mem_str_get_slen(str) + 2) <= SSTR_SIZE) {
        uchar slen = mem_str_get_slen(str);
        sstr->data[slen] = c;
        sstr->data[slen + 1] = '\0';
        mem_str_set_slen(str, (uchar)(slen + 1));
    } else if (!islong || (lstr->len + 2) > lstr->size) {
        size_t lsize = islong ? lstr->size : 32;
        char *data = mem_str_data(str);
        tmp = (char*)str_MALLOC(lsize * MEM_STRING_RESIZE);
        if (!tmp) return 1;

        /* random acts of optimization, remove local value, and directly */
        /* write to the string, which eliminates one write, and allocing */
        /* a potentially 64-bit long on the stack each time */
        lstr->len = islong ? lstr->len : mem_str_get_slen(str);
        memcpy(tmp, data, lstr->len);
        tmp[lstr->len++] = c;
        tmp[lstr->len] = '\0';

        if (islong)
            str_FREE(lstr->data);

        lstr->data = tmp;
        lstr->size = lsize * MEM_STRING_RESIZE;
        mem_str_set_long(str);
    } else {
        lstr->data[lstr->len++] = c;
        lstr->data[lstr->len] = '\0';
    }

    return 0;
}

char
mem_str_pop(mem_string *str)
{
    char ret = 0;
    dbg_assert((mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str)) > 0,
            "Index out of bounds");

    if (mem_str_is_long(str)) {
        ret = lstr->data[lstr->len - 1];
        lstr->len = lstr->len - 1;
        lstr->data[lstr->len] = '\0';
    } else {
        uchar slen = mem_str_get_slen(str);
        ret = sstr->data[slen - 1];
        mem_str_set_slen(str, (uchar)(slen - 1));
        sstr->data[slen - 1] = '\0';
    }

    return ret;
}

int
mem_str_cmp(mem_string *lhs, mem_string *rhs)
{
    char *d1, *d2;
    d1 = (mem_str_is_long(lhs) ? lhs->l.data : lhs->s.data);
    d2 = (mem_str_is_long(rhs) ? rhs->l.data : rhs->s.data);
    return strcmp(d1, d2);
}

int
mem_str_insert(mem_string *str, size_t pos, char const *s)
{
    char *tmp;
    size_t len;

    dbg_assert(s != NULL, "String must not be NULL");
    dbg_assert(pos <= (mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str)),
            "Index out of bounds");

    len = strlen(s);
    if (!mem_str_is_long(str) && (mem_str_get_slen(str) + len + 1) <= SSTR_SIZE) {
        uchar slen = mem_str_get_slen(str);
        memmove(sstr->data + pos + len, sstr->data + pos, slen - pos);
        memcpy(sstr->data + pos, s, len);
        mem_str_set_slen(str, (uchar)(slen + len));
    } else {
        if (!mem_str_is_long(str) || (lstr->len + len + 1) > lstr->size) {
            size_t llen = mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
            char* data = mem_str_data(str);
            size_t size = (llen + len + 1) * MEM_STRING_RESIZE;
            tmp = (char*)str_MALLOC(size);
            if (!tmp) return 1;

            memcpy(tmp, data, pos);
            memcpy(tmp + pos, s, len);
            memcpy(tmp + pos + len, data + pos, llen - pos);
            tmp[llen + len] = '\0';

            if (mem_str_is_long(str))
                str_FREE(lstr->data);

            lstr->data = tmp;
            lstr->len = llen + len + 1;
            lstr->size = size;
            mem_str_set_long(str);
        } else {
            memmove(lstr->data + pos + len, lstr->data + pos, lstr->len - pos);
            memcpy(lstr->data + pos, s, len);
            lstr->len = lstr->len + len;
        }
    }

    return 0;
}

int
mem_str_erase(mem_string *str, size_t pos, size_t count)
{
    dbg_assert(count > 0, "Count must not be zero");
    dbg_assert((pos + count) <= (mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str)), "Index out of bounds");

    if (mem_str_is_long(str)) {
        lstr->len = lstr->len - count;
        memmove(lstr->data + pos, lstr->data + pos + count, lstr->len - pos);
        lstr->data[lstr->len] = '\0';
    } else {
        uchar slen = (uchar)(mem_str_get_slen(str) - count);
        memmove(sstr->data + pos, sstr->data + pos + count, slen - pos);
        sstr->data[slen] = '\0';
        mem_str_set_slen(str, slen);
    }

    return 0;
}

int
mem_str_replace(mem_string *str, size_t pos, size_t count, char const *s)
{
    char *tmp;
    size_t len;
    int islong;

    dbg_assert(s != NULL, "String must not be NULL");
    dbg_assert(count > 0, "Count must not be zero");
    dbg_assert((pos + count) <= (mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str)), "Index out of bounds");

    len = strlen(s);
    islong = mem_str_is_long(str);
    if (!islong && ((mem_str_get_slen(str) - count) + len + 1) <= SSTR_SIZE) {
        uchar slen = mem_str_get_slen(str);
        memmove(sstr->data + pos + len, sstr->data + pos + count, slen - pos);
        memcpy(sstr->data + pos, s, len);
        mem_str_set_slen(str, (uchar)(slen - count + len));
    } else {
        if (!islong || ((lstr->len - count) + len + 1) > lstr->size) {
            size_t llen = islong ? lstr->len : mem_str_get_slen(str);
            char *data = mem_str_data(str);
            size_t size, nlen;

            nlen = llen - count + len;
            size = (lstr->len + 1) * MEM_STRING_RESIZE;
            tmp = (char*)str_MALLOC(size);
            if (!tmp) return 1;

            memcpy(tmp, data, pos);
            memcpy(tmp + pos, s, len);
            memcpy(tmp + pos + len, data + pos + count, llen - pos + count);
            tmp[nlen] = '\0';

            if (mem_str_is_long(str))
                str_FREE(lstr->data);

            lstr->data = tmp;
            lstr->len = nlen;
            lstr->size = size;
            mem_str_set_long(str);
        } else {
            memmove(lstr->data + pos + len, lstr->data + pos + count, lstr->len - pos);
            memcpy(lstr->data + pos, s, len);
            lstr->len = lstr->len - count + len;
        }
    }

    return 0;
}

mem_string *
mem_str_substr(mem_string *str, size_t pos, size_t count)
{
    mem_string *ret;
    size_t len;

    dbg_assert(count > 0, "Count must not be zero");

    len = mem_str_is_long(str) ? lstr->len : sstr->len;
    if (pos < len) {
        ret = mem_str_init(NULL, 0);
        if (!ret) return NULL;

        ret->l.data = (char*)str_MALLOC(count + 1);
        if (!ret->l.data) { mem_str_free(ret); return NULL; }

        memcpy(ret->l.data, mem_str_data(str) + pos, count);
        ret->l.data[count] = '\0';
        ret->l.len = count;
        ret->l.size = count + 1;
        mem_str_set_long(ret);

        return ret;
    }

    return NULL;
}

size_t
mem_str_reserve(mem_string *str, size_t size)
{
    char *tmp;
    size_t alloc, cs;

    cs = mem_str_is_long(str) ? lstr->size : SSTR_SIZE;
    if (size > cs) {
        size_t llen = mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
        alloc = size - cs;
        tmp = (char*)str_MALLOC(size);
        if (!tmp) return (size_t)-1;

        memcpy(tmp, mem_str_data(str), llen);
        tmp[llen] = '\0';

        if (mem_str_is_long(str))
            str_FREE(lstr->data);

        lstr->data = tmp;
        lstr->size = size;
        lstr->len = llen;
        mem_str_set_long(str);

        return alloc;
    }

    return 0;
}

size_t
mem_str_shrink(mem_string *str)
{
    char *tmp;
    size_t len, cs, rm;

    len = mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
    cs = mem_str_is_long(str) ? lstr->size : SSTR_SIZE;
    if (cs > (len + 1)) {
        tmp = (char*)str_MALLOC(len + 1);
        if (!tmp) return (size_t)-1;

        memcpy(tmp, mem_str_data(str), len);
        tmp[len] = '\0';

        rm = cs - len + 1;
        if (mem_str_is_long(str))
            str_FREE(lstr->data);

        lstr->data = tmp;
        lstr->size = len + 1;
        mem_str_set_long(str);

        return rm;
    }

    return 0;
}

int
mem_str_empty(mem_string *str)
{
    return (mem_str_is_long(str) ? lstr->len: mem_str_get_slen(str)) == 0;
}

size_t
mem_str_length(mem_string *str)
{
    return mem_str_is_long(str) ? lstr->len : mem_str_get_slen(str);
}

size_t
mem_str_maxsize(mem_string *str)
{
    return mem_str_is_long(str) ? lstr->size : SSTR_SIZE;
}

INLINE int
mem_str_is_long(mem_string *str)
{
    return str->s.len & 1;
}

INLINE void
mem_str_set_long(mem_string *str)
{
    str->s.len |= 1;
}

INLINE void
mem_str_set_slen(mem_string *str, uchar len)
{
    str->s.len = (uchar)(len << 1);
}

INLINE uchar
mem_str_get_slen(mem_string *str)
{
    return str->s.len >> 1;
}

INLINE char *
mem_str_data(mem_string *str)
{
    return (mem_str_is_long(str) ? str->l.data : str->s.data);
}

