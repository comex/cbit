#ifndef ACTUALLY_STR_REALLOC
#include "vec.h"
#endif
#include <stdio.h>
#include <stdlib.h>

void vec_realloc_internal(struct vec_internal *vi, size_t new_capacity
                          #ifndef ACTUALLY_STR_REALLOC
                          , size_t esize
                          #endif
                          ) {
    size_t new_size;
    cbit_dassert(!VEC_DBG_ISREADONLY(vi));
    if (new_capacity > (vec_count_t) -2
    #ifndef ACTUALLY_STR_REALLOC
        || new_capacity > ((size_t) -1) / esize
    #endif
        )
        cbit_panic("vec_realloc_internal: capacity too large (%llu)\n",
                   (long long) new_capacity);
    if (new_capacity == 0) {
        if (VEC_CAPACITY_IS_FIXED(vi->capacity))
            free(vi->els);
        vi->els = NULL;
        vi->capacity = 0;
        return;
    }
    new_capacity = cbit_max(new_capacity, 8);
    new_capacity = (new_capacity + 1) & ~1;
    new_size = new_capacity * esize;
    #ifdef ACTUALLY_STR_REALLOC
        new_size++;
    #endif
    if (VEC_CAPACITY_IS_FIXED(vi->capacity)) {
        #ifdef ACTUALLY_STR_REALLOC
            void *new_buf = calloc(new_size, 1);
        #else
            void *new_buf = malloc(new_size);
        #endif
        size_t min_cap = cbit_min(new_capacity, vi->capacity);
        memcpy(new_buf, vi->els, min_cap * esize);
        vi->els = (char *) new_buf;
    } else {
        void *new_buf = realloc(vi->els, new_size);
        #ifdef ACTUALLY_STR_REALLOC
            size_t min_cap = cbit_min(new_capacity, vi->capacity);
            memset((char *) new_buf + min_cap, 0, new_size - min_cap);
        #endif
        vi->els = (char *) new_buf;
    }
    vi->capacity = new_capacity;
}
void vec_realloc_internal_as_necessary(struct vec_internal *vi,
                                       size_t min_capacity
                                       #ifndef ACTUALLY_STR_REALLOC
                                       , size_t esize
                                       #endif
                                       ) {
    if (min_capacity > vi->capacity) {
        size_t capacity = cbit_max(min_capacity, 15);
        capacity = cbit_max(capacity, safe_mul(vi->capacity, 2));
        vec_realloc_internal(vi, capacity
        #ifndef ACTUALLY_STR_REALLOC
            , esize
        #endif
        );
    } else if (!VEC_CAPACITY_IS_FIXED(vi->capacity) &&
               min_capacity < vi->capacity / 3) {
        vec_realloc_internal(vi, vi->capacity / 3
        #ifndef ACTUALLY_STR_REALLOC
                             , esize
        #endif
        );
    }
}
