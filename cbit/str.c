#include "str.h"

#define ACTUALLY_STR_REALLOC
#define vec_internal str
#define vec_realloc_internal str_realloc
#define vec_realloc_internal_as_necessary str_realloc_internal_as_necessary
#define VEC_DBG_ISREADONLY STR_DBG_ISREADONLY
#define VEC_CAPACITY_IS_FIXED STR_CAPACITY_IS_FIXED
#define esize 1
#include "vec.c"

char str_empty_els = '\0';

str str_fmt(const char *format, ...) {
    char *out;
    str outs;
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = vasprintf(&out, format, ap);
    va_end(ap);
    if (ret == -1)
        cbit_panic("str_fmt: out of memory\n");
    outs.length = outs.capacity = ret;
    outs.els = out;
    return outs;
}

void str_fread(str *CBIT_RESTRICT out, FILE *CBIT_RESTRICT fp, size_t limit) {
    size_t offset;
    if (out->capacity < 8192)
        str_realloc(out, 8192);
    offset = out->length;
    while (1) {
        size_t desired = cbit_min(out->capacity, limit) - offset;
        if (desired == 0)
            break;
        size_t res = fread(out->els + offset, 1, desired, fp);
        offset += res;
        if (res < desired)
            break;
        str_realloc(out, out->capacity * 2);
    }
    out->length = offset;
}
