#pragma once
#include "vec.h"
DECL_VEC(char, char);
typedef struct vec_char str;

#include <string.h>
#include <stdarg.h>

#define S(strlit) ((str) {.length = sizeof(strlit)-1, .capacity = 0, .els = (strlit)})

#define str_appendchar vec_append_char
#define str_resize vec_resize_char
#define str_realloc vec_realloc_char
#define str_free_storage vec_free_storage_char
typedef VEC_STORAGE_CAPA(char, 16) str_storage;
#define STR_STORAGE_CAPA(capa) VEC_STORAGE_CAPA(char, capa)

#define STR_FOREACH(str, idx_var, ptr_var) \
    VEC_FOREACH(str, idx_var, ptr_var, char)

static inline str str_withc(char *cstr) {
    size_t len = strlen(cstr);
    return (str) {.length = len, .capacity = len, .els = cstr};
}

str *str_replace(const str *haystack, const str *search, const str *replace);

/* non-typesafe format (just a wrapper around asprintf) */
str str_fmt(const char *format, ...);
