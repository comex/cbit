#pragma once
#include "misc.h"
#include <stdlib.h>
#include <string.h>

#ifndef vec_count_t
typedef uint32_t vec_count_uint32_t;
#define vec_count_t vec_count_uint32_t
#endif

struct vec_internal {
    vec_count_t length;
    vec_count_t capacity;
    void *els;
};

#ifdef __cplusplus
extern "C" {
#endif

void vec_realloc_internal(struct vec_internal *vi, size_t new_capacity,
                          size_t esize);
void vec_realloc_internal_as_necessary(struct vec_internal *vi,
                                       size_t min_capacity,
                                       size_t esize);
#ifdef __cplusplus
}
#endif

#define DECL_VEC(ty, name) \
    typedef ty __VEC_TY_##name; \
    struct vec_##name { \
        union { \
            struct vec_internal vi; \
            struct { \
                vec_count_t length; \
                vec_count_t capacity; \
                VEC_TY(name) *els; \
            }; \
        }; \
    }; \
    UNUSED_STATIC_INLINE \
    void vec_free_storage_##name(struct vec_##name *v) { \
        if (!VEC_CAPACITY_IS_FIXED(v->capacity)) \
            free(v->els); \
    } \
    UNUSED_STATIC_INLINE \
    struct vec_##name vec_borrow##name(VEC_TY(name) *els, size_t length) { \
        struct vec_##name v; \
        v.length = length; \
        v.capacity = 0; \
        v.els = els; \
        return v; \
    } \
    UNUSED_STATIC_INLINE \
    void vec_realloc_##name(struct vec_##name *v, size_t new_capacity) { \
        vec_realloc_internal(&v->vi, new_capacity, sizeof(v->els[0])); \
    } \
    UNUSED_STATIC_INLINE \
    void vec_resize_##name(struct vec_##name *v, size_t new_length) { \
        if (new_length > v->capacity || new_length < v->capacity / 3) \
            vec_realloc_internal_as_necessary(&v->vi, new_length, \
                                              sizeof(v->els[0])); \
        v->length = new_length; \
    } \
    UNUSED_STATIC_INLINE \
    VEC_TY(name) *vec_appendp_##name(struct vec_##name *v) { \
        size_t i, new_length; \
        cbit_dassert(!VEC_DBG_ISREADONLY(v)); \
        i = v->length; \
        new_length = i + 1; \
        if (i >= v->capacity) \
            vec_realloc_internal_as_necessary(&v->vi, new_length, sizeof(v->els[0])); \
        v->length = new_length; \
        return &v->els[i]; \
    } \
    UNUSED_STATIC_INLINE \
    VEC_TY(name) *vec_appendp_n_##name(struct vec_##name *v, size_t count) { \
        size_t i, new_length; \
        cbit_dassert(!VEC_DBG_ISREADONLY(v)); \
        i = v->length; \
        new_length = safe_add(v->length, count); \
        if (new_length > v->capacity) \
            vec_realloc_internal_as_necessary(&v->vi, new_length, sizeof(v->els[0])); \
        v->length = new_length; \
        return &v->els[i]; \
    } \
    UNUSED_STATIC_INLINE \
    void vec_append_##name(struct vec_##name *v, VEC_TY(name) val) { \
        *vec_appendp_##name(v) = val; \
    } \
    UNUSED_STATIC_INLINE \
    VEC_TY(name) vec_pop_##name(struct vec_##name *v) { \
        size_t i; \
        VEC_TY(name) ret; \
        cbit_dassert(!VEC_DBG_ISREADONLY(v)); \
        i = v->length - 1; \
        ret = v->els[i]; \
        if (!VEC_CAPACITY_IS_FIXED(v->capacity) && i < v->capacity / 3) \
            vec_realloc_internal_as_necessary(&v->vi, i, sizeof(v->els[0])); \
        v->length = i; \
       return ret; \
    } \
    UNUSED_STATIC_INLINE \
    void vec_concat_##name(struct vec_##name *v1, const struct vec_##name *v2) { \
        size_t l1 = v1->length, l2 = v2->length; \
        vec_resize_##name(v1, safe_add(l1, l2)); \
        memcpy(&v1->els[l1], v2->els, l2 * sizeof(v1->els[0])); \
    } \
    UNUSED_STATIC_INLINE \
    void vec_add_space_##name(struct vec_##name *v, size_t idx, size_t num) { \
        /* todo optimize */ \
        size_t orig = v->length; \
        vec_resize_##name(v, orig + num); \
        memmove(&v->els[idx + num], &v->els[idx], \
                (orig - idx) * sizeof(v->els[0])); \
    } \
    UNUSED_STATIC_INLINE \
    void vec_remove_##name(struct vec_##name *v, size_t idx, size_t num) { \
        /* todo optimize */ \
        size_t orig; \
        cbit_dassert(!VEC_DBG_ISREADONLY(v)); \
        orig = v->length; \
        memmove(&v->els[idx], &v->els[idx + num], \
                (orig - (idx + num)) * sizeof(v->els[0])); \
        vec_resize_##name(v, orig - num); \
    } \
    UNUSED_STATIC_INLINE \
    struct vec_##name vec_copy_##name(const struct vec_##name *v) { \
        struct vec_##name ret = VEC_INITER; \
        size_t len = v->length; \
        vec_resize_##name(&ret, len); \
        memcpy(ret.els, v->els, len); \
        return ret; \
    } \
    typedef VEC_STORAGE_CAPA(name, 5) vec_storage_##name; \
    typedef char __plz_end_DECL_VEC_with_semicolon_##name

#define VEC_TY(name) __VEC_TY_##name

#define VEC_STORAGE_CAPA(name, n) \
    struct { \
        struct vec_##name v; \
        VEC_TY(name) storage[(n)]; \
    }

#define VEC_CAPACITY_IS_FIXED(len) \
    (((len) & 1) || ((len) < 8))

#define VEC_STORAGE_INIT(vs, name) do { \
    struct vec_##name *v = &(vs)->v; \
    v->length = 0; \
    CBIT_STATIC_ASSERT( \
        VEC_CAPACITY_IS_FIXED(sizeof((vs)->storage) / sizeof(VEC_TY(name))), \
        "fixed vec_storage size should be odd or < 8" \
    ); \
    v->capacity = (sizeof((vs)->storage) / sizeof(VEC_TY(name))); \
    v->els = (vs)->storage; \
} while (0)

#define VEC_STORAGE_INITER(vs, name) \
    {{0, \
      (CBIT_STATIC_ASSERT_EXPR( \
        VEC_CAPACITY_IS_FIXED(sizeof((vs)->storage) / sizeof((vs)->storage[0])), \
        "fixed vec_storage size should be odd or < 8" \
       ), \
       (sizeof((vs)->storage) / sizeof((vs)->storage[0]))), \
      (vs)->storage \
    }}

#define VEC_INIT(v) do { \
    struct vec_internal *vi = &(v)->vi; \
    vi->length = 0; \
    vi->capacity = 0; \
    vi->els = 0; \
} while (0)

#define VEC_INITER \
    {{0, 0, 0}}

#define VEC_DBG_ISREADONLY(v) ((v)->length && !(v)->capacity)

/* guaranteed to *not* cache vec->length - pretty simple */

#define VEC_FOREACH(vec, idx_var, ptr_var, name) \
    LET(struct vec_##name *__vfe_v = (vec)) \
        for (size_t idx_var = 0; idx_var < __vfe_v->length; idx_var++) \
            LET_LOOP(ptr_var = &__vfe_v->els[idx_var])

#define autofree_vec(name) \
    __attribute__((cleanup(vec_free_storage_##name))) struct vec_##name

