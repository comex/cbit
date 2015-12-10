#pragma once
#define CBIT_STR_H
#include "misc.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef vec_count_t
typedef uint32_t vec_count_uint32_t;
#define vec_count_t vec_count_uint32_t
#endif

/* this is like a vec but not quite the same because it guarantees a trailing
 * nul byte */

typedef struct str {
    vec_count_t length;
    vec_count_t capacity;
    char *els;
} str;

extern char str_empty_els; /* = '\0' */

void str_realloc(str *v, size_t new_capacity);
void str_realloc_internal_as_necessary(str *s, size_t min_capacity);

#define STR_CAPACITY_IS_FIXED(len) \
    (((len) & 1) || ((len) < 8))

#define STR_DBG_ISREADONLY(v) ((v)->length && !(v)->capacity)

#define STR_INIT(_s) do { \
    str *s = (_s); \
    s->length = 0; \
    s->capacity = 0; \
    vi->els = &str_empty_els; \
}
#define STR_INITER {0, 0, &str_empty_els}

UNUSED_STATIC_INLINE
void str_free_storage(str *v) {
    if (!STR_CAPACITY_IS_FIXED(v->capacity))
        free(v->els);
}
UNUSED_STATIC_INLINE
str str_borrow(const char *els, size_t length) {
    str v;
    v.length = length;
    v.capacity = 0;
    v.els = (char *) els;
    return v;
}
UNUSED_STATIC_INLINE
str str_borrowc(const char *cstr) {
    return str_borrow(cstr, strlen(cstr));
}
UNUSED_STATIC_INLINE
str str_withc(char *cstr) {
    size_t length = strlen(cstr);
    str s;
    s.length = s.capacity = length;
    s.els = cstr;
    return s;
}
UNUSED_STATIC_INLINE
void str_resize(str *v, size_t new_length) {
    if (new_length > v->capacity || new_length < v->capacity / 3)
        str_realloc_internal_as_necessary(v, new_length);
    v->length = new_length;
}
UNUSED_STATIC_INLINE
char *str_appendp_n(str *v, size_t count) {
    size_t i, new_length;
    cbit_dassert(!STR_DBG_ISREADONLY(v));
    i = v->length;
    new_length = safe_add(v->length, count);
    if (new_length > v->capacity)
        str_realloc_internal_as_necessary(v, new_length);
    v->length = new_length;
    return &v->els[i];
}
UNUSED_STATIC_INLINE
void str_appendchar(str *v, char c) {
    *str_appendp_n(v, 1) = c;
}
UNUSED_STATIC_INLINE
void str_concat(struct str *v1, const struct str *v2) {
    size_t l1 = v1->length, l2 = v2->length;
    str_resize(v1, safe_add(l1, l2));
    memcpy(&v1->els[l1], v2->els, l2 * sizeof(v1->els[0]));
}
UNUSED_STATIC_INLINE
void str_remove(str *v, size_t idx, size_t num) {
    /* todo optimize */
    size_t orig;
    cbit_dassert(!STR_DBG_ISREADONLY(v));
    orig = v->length;
    memmove(&v->els[idx], &v->els[idx + num],
            (orig - (idx + num)) * sizeof(v->els[0]));
    str_resize(v, orig - num);
}
UNUSED_STATIC_INLINE
str str_copy(const str *v) {
    str ret = STR_INITER;
    size_t len = v->length;
    str_resize(&ret, len);
    memcpy(ret.els, v->els, len);
    return ret;
}

#if __STDC_VERSION__ >= 199901L
#define S(strlit) (&((str) {.length = sizeof(strlit)-1, .capacity = 0, \
                            .els = (strlit)}))
#endif
#define Sv(strlit) str_borrow((strlit), sizeof(strlit)-1)

#define STR_STORAGE_CAPA(n) \
    struct { \
        str v; \
        char storage[(n)]; \
    }
typedef STR_STORAGE_CAPA(15) str_storage;

#define STR_STORAGE_INIT(vs) do { \
    str *v = &(vs)->v; \
    v->length = 0; \
    CBIT_STATIC_ASSERT( \
        STR_CAPACITY_IS_FIXED(sizeof((vs)->storage)), \
        "fixed str_storage size should be odd or < 8" \
    ); \
    v->capacity = sizeof((vs)->storage); \
    v->els = (vs)->storage; \
} while (0)

#define STR_STORAGE_INITER(vs) \
    {{0, \
      (CBIT_STATIC_ASSERT_EXPR( \
        STR_CAPACITY_IS_FIXED(sizeof((vs)->storage)), \
        "fixed str_storage size should be odd or < 8" \
       ), \
       sizeof((vs)->storage)), \
      (vs)->storage \
    }}

#define STR_FOREACH(str, idx_var, ptr_var) \
    LET(str *__sfe_s = (str)) \
        for (size_t idx_var = 0; idx_var < __sfe_s->length; idx_var++) \
            LET_LOOP(ptr_var = &__sfe_s->els[idx_var])


str *str_replace(const str *haystack, const str *search, const str *replace);

/* non-typesafe format (just a wrapper around asprintf) */
str str_fmt(const char *format, ...);

UNUSED_STATIC_INLINE size_t
str_fwrite(FILE *fp, const str *str) {
    return fwrite(str->els, 1, str->length, fp);
}

UNUSED_STATIC_INLINE size_t
str_print(const str *str) {
    return str_fwrite(stdout, str);
}
