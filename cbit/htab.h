#pragma once
#include "misc.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct htab_internal {
    size_t length;
    size_t capacity;
    void *base;
    char hi_storage[1]; /* see vec.h */
};

/* Declare the helper functions for a htab key type without static - put this
 * in the .h and IMPL_HTAB_KEY in a .c. */
#define DECL_EXTERN_HTAB_KEY( \
    name, \
    /* The key type. */ \
    key_ty, \
    /* bool null_func(const ty *) - check whether the stored key is a \
     * special invalid marker. \
     * May be a macro. */ \
    null_func, \
    /* uint8_t nil_byte - which byte to memset to initially. \
     * null_func(<all bytes nil_byte>) must be true. */ \
    nil_byte \
) \
    DO_DECL_HTAB_KEY(name, key_ty, null_func, nil_byte, extern)

/* Declare and implement the helper functions static inline.  For the argument
 * meanings, see IMPL_HTAB_KEY. */
#define DECL_STATIC_HTAB_KEY( \
    name, \
    key_ty, \
    hash_func, \
    eq_func, \
    null_func, \
    nil_byte \
) \
    DO_DECL_HTAB_KEY(name, key_ty, null_func, nil_byte, UNUSED_STATIC_INLINE); \
    IMPL_HTAB_KEY(name, hash_func, eq_func)

#define DO_DECL_HTAB_KEY(name, key_ty, null_func, nil_byte, func_decl) \
    typedef key_ty __htab_key_key_ty_##name; \
    enum { __htab_key_nil_byte_##name = (nil_byte) }; \
    DO_DECL_HTAB_KEY_(name, __htab_key_key_ty_##name, null_func, func_decl)
#define DO_DECL_HTAB_KEY_(name, key_ty, null_func, func_decl) \
    UNUSED_STATIC_INLINE \
    bool __htab_key_is_null_##name(const key_ty *bucket) { \
        return null_func(bucket); \
    } \
    func_decl \
    void *__htab_key_lookup_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                   const key_ty *CBIT_RESTRICT key, \
                                   size_t entry_size, \
                                   bool add); \
    func_decl \
    void __htab_key_removeat_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                    void *op, \
                                    size_t entry_size); \
    func_decl \
    void __htab_key_memset_##name(void *ptr, size_t size); \
    func_decl \
    void __htab_key_resize_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                  size_t size, \
                                  size_t entry_size) \

#define IMPL_HTAB_KEY(name, \
    /* size_t hash_func(const ty *) - hash. \
     * May be a macro. */ \
    hash_func, \
    /* bool eq_func(const ty *stored, const ty *queried) - check whether the \
     * stored key is equal to the requested key (which is assumed to be valid). \
     * May be a macro. */ \
    eq_func \
) \
    DO_IMPL_HTAB_KEY(name, __htab_key_key_ty_##name, hash_func, \
                     eq_func)
#define DO_IMPL_HTAB_KEY(name, key_ty, hash_func, eq_func) \
    void __htab_key_resize_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                  size_t capacity, \
                                  size_t entry_size); \
    void *__htab_key_lookup_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                   const key_ty *CBIT_RESTRICT key, \
                                   size_t entry_size, \
                                   bool add) { \
        size_t capacity, i, hash; \
        char *buckets; \
        if (add && \
            hi->capacity * 2 <= hi->length * 3) \
            __htab_key_resize_##name(hi, hi->capacity * 2, entry_size); \
        capacity = hi->capacity; \
        if (capacity == 0) \
            return NULL; \
        hash = (hash_func(key)) % capacity; \
        i = hash; \
        buckets = (char *) hi->base; \
        do { \
            key_ty *bucket = (key_ty *) (buckets + i * entry_size); \
            if (__htab_key_is_null_##name(bucket)) { \
                if (add) { \
                    hi->length++; \
                    return bucket; \
                } else { \
                    return NULL; \
                } \
            } \
            if (eq_func(bucket, key)) \
                return bucket; \
        } while (i = (i + 1) == capacity ? 0 : (i + 1), i != hash); \
        return NULL; \
    } \
    \
    /* a bit inefficient */ \
    void __htab_key_removeat_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                    void *op, \
                                    size_t entry_size) { \
        size_t capacity = hi->capacity; \
        char *buckets = (char *) hi->base; \
        key_ty *end = (key_ty *) ((char *) buckets + capacity * entry_size); \
        key_ty *hole = (key_ty *) op; \
        key_ty *cur = hole; \
        while (1) { \
            size_t cur_hash; \
            key_ty *cur_chain_first; \
            bool cf_after_hole; \
            if (cur == end) \
                cur = (key_ty *) buckets; \
            cur = (key_ty *) ((char *) cur + entry_size); \
            if (cur == hole || __htab_key_is_null_##name(cur)) { \
                /* all of the elements in this chain have starting \
                 * positions (hashes) strictly 'after' the hole position, \
                 * so we can't move any of them into the hole.  but that \
                 * also means it's safe to just erase the hole. */ \
                memset(hole, __htab_key_nil_byte_##name, sizeof(key_ty)); \
                break; \
            } \
            cur_hash = (hash_func(cur)) % capacity; \
            cur_chain_first = (key_ty *) \
                ((char *) buckets + cur_hash * entry_size); \
            cf_after_hole /* cyclically */ = hole <= cur ? \
                (hole < cur_chain_first) : \
                (cur < cur_chain_first && cur_chain_first <= hole); \
            if (!cf_after_hole) { \
                memcpy(hole, cur, entry_size); \
                hole = cur; \
            } \
        } \
        hi->length--; \
    } \
    void __htab_key_memset_##name(void *ptr, size_t size) { \
        memset(ptr, __htab_key_nil_byte_##name, size); \
    } \
    void __htab_key_resize_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                  size_t size, \
                                  size_t entry_size) { \
        size_t old_size = hi->capacity * entry_size; \
        size_t new_size = safe_mul(size, entry_size); \
        void *new_buf = malloc(new_size); \
        size_t i; \
        struct htab_internal temp; \
        memset(new_buf, __htab_key_nil_byte_##name, new_size); \
        temp.length = 0; \
        temp.capacity = size; \
        temp.base = new_buf; \
        for (i = 0; i < old_size; i += entry_size) { \
            key_ty *bucket = (key_ty *) ((char *) hi->base + i); \
            if (!__htab_key_is_null_##name(bucket)) { \
                memcpy( \
                    __htab_key_lookup_##name(&temp, bucket, entry_size, \
                                             true), \
                    bucket, \
                    entry_size); \
            } \
        } \
        hi->capacity = size; \
        if (hi->base != hi->hi_storage) \
            free(hi->base); \
        hi->base = new_buf; \
    } \
    typedef char __htab_want_semicolon_here_##name

#define DECL_HTAB( \
    name, \
    /* The name parameter to DECL_HTAB_KEY */ \
    key_name, \
    value_ty) \
    typedef __htab_key_key_ty_##key_name __htab_key_ty_##name; \
    typedef value_ty __htab_value_ty_##name; \
    enum { __htab_nil_byte_##name = __htab_key_nil_byte_##key_name }; \
    \
    DO_DECL_HTAB(name, \
                 __htab_key_ty_##name, \
                 __htab_value_ty_##name, \
                 struct htab_bucket_##name, \
                 struct htab_##name, \
                 key_name)

#define DO_DECL_HTAB(name, key_ty, value_ty, bucket_ty, htab_ty, key_name) \
    bucket_ty { \
        key_ty key; \
        value_ty value; \
    }; \
    htab_ty { \
        union { \
            struct htab_internal hi; \
            struct { \
                size_t length; \
                size_t capacity; \
                bucket_ty *base; \
                bucket_ty storage[1]; \
            }; \
        }; \
    }; \
    UNUSED_STATIC_INLINE \
    bucket_ty *htab_getbucket_##name(htab_ty *CBIT_RESTRICT ht, \
                                     const key_ty *CBIT_RESTRICT key) { \
        return (bucket_ty *) \
               __htab_key_lookup_##key_name(&ht->hi, key, sizeof(bucket_ty), \
                                            false); \
    } \
    UNUSED_STATIC_INLINE \
    value_ty *htab_getp_##name(const htab_ty *CBIT_RESTRICT ht, \
                               const key_ty *CBIT_RESTRICT key) { \
        bucket_ty *bucket = htab_getbucket_##name((htab_ty *) ht, key); \
        return bucket ? &bucket->value : NULL; \
    } \
    UNUSED_STATIC_INLINE \
    bucket_ty *htab_setbucket_##name(htab_ty *CBIT_RESTRICT ht, \
                                     const key_ty *CBIT_RESTRICT key) { \
        return (bucket_ty *) \
               __htab_key_lookup_##key_name(&ht->hi, key, sizeof(bucket_ty), \
                                            true); \
    } \
    UNUSED_STATIC_INLINE \
    value_ty *htab_setp_##name(const htab_ty *CBIT_RESTRICT ht, \
                               const key_ty *CBIT_RESTRICT key, \
                               bool *new_p) { \
        bucket_ty *bucket = htab_setbucket_##name((htab_ty *) ht, key); \
        bool is_new = false; \
        if (__htab_key_is_null_##key_name(&bucket->key)) { \
            bucket->key = *key; \
            is_new = true; \
        } else { \
            is_new = false; \
        } \
        if (new_p) \
            *new_p = is_new; \
        return &bucket->value; \
    } \
    UNUSED_STATIC_INLINE \
    bool htab_remove_##name(htab_ty *CBIT_RESTRICT ht, \
                            const key_ty *CBIT_RESTRICT key) { \
        void *op = __htab_key_lookup_##key_name(&ht->hi, key, sizeof(bucket_ty), \
                                                false); \
        if (!op) \
            return false; \
        __htab_key_removeat_##key_name(&ht->hi, op, sizeof(bucket_ty)); \
        return true; \
    } \
    UNUSED_STATIC_INLINE \
    void htab_removeat_##name(htab_ty *CBIT_RESTRICT ht, bucket_ty *op) { \
        __htab_key_removeat_##key_name(&ht->hi, op, sizeof(bucket_ty)); \
    } \
    UNUSED_STATIC_INLINE \
    void __htab_memset_##name(void *ptr, size_t size) { \
        __htab_key_memset_##key_name(ptr, size); \
    } \
    UNUSED_STATIC_INLINE \
    bool __htab_is_null_##name(const key_ty *bucket) { \
        return __htab_key_is_null_##key_name(bucket); \
    } \
    UNUSED_STATIC_INLINE \
    void htab_resize_##name(htab_ty *ht, size_t size) { \
        __htab_key_resize_##key_name(&ht->hi, size, sizeof(bucket_ty)); \
    } \
    UNUSED_STATIC_INLINE \
    void htab_free_storage_##name(htab_ty *ht) { \
        if (ht->base != ht->storage) \
            free(ht->base); \
    } \
    typedef char __plz_end_decl_htab_with_semicolon_##name

#define HTAB_STORAGE_CAPA(name, n) \
   struct { \
      struct htab_##name h; \
      struct htab_bucket_##name rest[(n)-1]; \
   }

#define HTAB_STORAGE(name) \
    HTAB_STORAGE_CAPA(name, 5)

#define HTAB_STORAGE_INIT(hs, name) do { \
    struct htab_##name *h = &(hs)->h; \
    h->length = 0; \
    h->capacity = (sizeof((hs)->rest) / \
                   sizeof(struct htab_bucket_##name)) + 1; \
    h->base = h->storage; \
    __htab_memset_##name(h->base, \
                         h->capacity * sizeof(struct htab_bucket_##name)); \
} while (0)

/* only works if nil_byte is 0, and doesn't work in C89 */
#define HTAB_STORAGE_INITER(hs, name) \
    {{( \
       0, \
       CBIT_STATIC_ASSERT_EXPR(__htab_nil_byte_##name == 0, \
            "HTAB_STORAGE_INITER only works with nil_byte == 0"), \
       0), \
      (sizeof((hs)->rest) / sizeof(struct htab_bucket_##name)) + 1, \
      (hs)->h.storage \
    }}

#define HTAB_FOREACH(ht, key_var, val_var, name) \
    LET(struct htab_##name *__htfe_ht = (ht)) \
        for (size_t __htfe_bucket = 0; \
             __htfe_bucket < __htfe_ht->capacity; \
             __htfe_bucket++) \
            if(__htab_is_null_##name(&__htfe_ht->base[__htfe_bucket].key)) \
                continue; \
            else \
                LET_LOOP(key_var = &__htfe_ht->base[__htfe_bucket].key) \
                LET_LOOP(val_var = &__htfe_ht->base[__htfe_bucket].value)

/* Note: this violates strict aliasing if the hash/eq/null functions actually
 * dereference the key! */
#define DECL_HTAB_KEY_ALIAS(name, key_ty, origname) \
    typedef key_ty __htab_key_key_ty_##name; \
    DO_DECL_HTAB_KEY_ALIAS(name, __htab_key_key_ty_##name, origname, __htab_key_key_ty_##origname)
#define DO_DECL_HTAB_KEY_ALIAS(name, key_ty, origname, origkey_ty) \
    UNUSED_STATIC_INLINE \
    void *__htab_key_lookup_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                   const key_ty *CBIT_RESTRICT key, \
                                   size_t entry_size, \
                                   bool add) { \
        return __htab_key_lookup_##origname(hi, (const origkey_ty *) key, \
                                            entry_size, add); \
    } \
    UNUSED_STATIC_INLINE \
    void __htab_key_removeat_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                    void *op, \
                                    size_t entry_size) { \
        __htab_key_removeat_##origname(hi, op, entry_size); \
    } \
    UNUSED_STATIC_INLINE \
    void __htab_key_memset_##name(void *ptr, size_t size) { \
        __htab_key_memset_##origname(ptr, size); \
    } \
    UNUSED_STATIC_INLINE \
    bool __htab_key_is_null_##name(const key_ty *bucket) { \
        return __htab_key_is_null_##origname((const origkey_ty *) bucket); \
    } \
    UNUSED_STATIC_INLINE \
    void __htab_key_resize_##name(struct htab_internal *CBIT_RESTRICT hi, \
                                  size_t size, \
                                  size_t entry_size) { \
        __htab_key_resize_##origname(hi, size, entry_size); \
    } \
    typedef char __plz_end_decl_htab_key_alias_with_semicolon_##name

typedef char __htab_ptrbytes[sizeof(void *)];
UNUSED_STATIC_INLINE
size_t __htab_pb_hash(const __htab_ptrbytes *pb) {
    size_t size;
    memcpy(&size, pb, sizeof(size));
    if (sizeof(size) == 4)
        return (size_t) ((size * 0xdeadbeef) % 4294967291);
    else
        return (size_t) ((size * 0xdeadbeefdeadbeefull) % 18446744073709551557ull);
}
UNUSED_STATIC_INLINE
bool __htab_pb_eq(const __htab_ptrbytes *pb, const __htab_ptrbytes *pb2) {
    return !memcmp(pb, pb2, sizeof(__htab_ptrbytes));
}
UNUSED_STATIC_INLINE
bool __htab_pb_null(const __htab_ptrbytes *pb) {
    static const __htab_ptrbytes zeroes = {0};
    return !memcmp(pb, &zeroes, sizeof(__htab_ptrbytes));
}
DECL_STATIC_HTAB_KEY(__htab_ptrbytes, __htab_ptrbytes, __htab_pb_hash,
                     __htab_pb_eq, __htab_pb_null, 0);
#undef pb_hash
#undef pb_eq
#undef pb_null
