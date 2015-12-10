#pragma once
#define CBIT_OPT_H
#include "misc.h"
#include <stdbool.h>

#define __DECL_OPT_KIND_primitive(name) \
    __DECL_OPT_KIND_normal(name) \
    UNUSED_STATIC_FORCEINLINE \
    bool opt_eq_##name(opt_##name a, opt_##name b) { \
        return a.have == b.have && (!a.have || a.val == b.val); \
    }

#define __DECL_OPT_KIND_normal(name) \
    typedef struct __opt_struct_##name { \
        bool have; \
        __OPT_BASE_##name val; \
    } opt_##name; \
    UNUSED_STATIC_FORCEINLINE \
    opt_##name some_##name(__OPT_BASE_##name val) { \
        opt_##name o; \
        o.have = true; \
        o.val = val; \
        return o; \
    } \
    UNUSED_STATIC_FORCEINLINE \
    opt_##name none_##name(void) { \
        opt_##name o; \
        o.have = false; \
        return o; \
    } \

#define __DECL_OPT_KIND_nonzero(name) \
    typedef union __opt_struct_ptrwrap_##name { \
        __OPT_BASE_##name have; \
        __OPT_BASE_##name val; \
    } opt_##name; \
    UNUSED_STATIC_FORCEINLINE \
    opt_##name some_##name(__OPT_BASE_##name val) { \
        cbit_serious_assert(val); \
        opt_##name o; \
        o.val = val; \
        return o; \
    } \
    UNUSED_STATIC_FORCEINLINE \
    opt_##name none_##name(void) { \
        opt_##name o; \
        o.val = 0; \
        return o; \
    }

#define DECL_OPT_ALWAYS(ty, name, kind) \
    typedef ty __OPT_BASE_##name; \
    typedef ty __OPT_CHECK_##name##_kind_##kind; \
    __DECL_OPT_KIND_##kind(name) \
    UNUSED_STATIC_INLINE \
    __OPT_BASE_##name unwrap_##name(opt_##name o) { \
        if (!o.have) \
            cbit_panic("unwrap fail"); \
        return o.val; \
    } \
    typedef char __PLZ_END_DECL_OPT_ALWAYS_WITH_SEMICOLON_##name

/* ext */

#define DECL_OPT_SUPPORTED CBIT_HAVE_MS_EXTS
#if DECL_OPT_SUPPORTED

#define DECL_OPT(ty, name, kind) \
    __if_not_exists (__OPT_BASE_##name) { \
        DECL_OPT_ALWAYS(ty, name, kind); \
    }
/*
    __if_exists (__OPT_BASE_##name) { \
        CBIT_STATIC_ASSERT((__OPT_CHECK_##name_kind_##kind *) 0 \
                           == (ty *) 0, "DECL_OPT mismatch"); \
    }
*/

#define DECL_OPT_MAYBE_ALWAYS DECL_OPT
#else
#define DECL_OPT(ty, name, kind) \
    CBIT_STATIC_ASSERT(0, "DECL_OPT not supported, try with -fms-extensions on Clang or give up with GCC"); \
    DECL_OPT_ALWAYS(ty, name, kind)

#define DECL_OPT_MAYBE_ALWAYS DECL_OPT_ALWAYS
#endif

#if CBIT_HAVE_GNU_EXTS
#define TRY(rty, expr...) ({ \
    CBIT_AUTO(_res, (expr)); \
    if (!_res.have) \
        return none_##rty(); \
    _res.val; \
})
#define opt_eq(a, b) ({ \
    CBIT_AUTO(_a, a); \
    CBIT_AUTO(_b, b); \
    _a.have == _b.have && (!_a.have || _a.val == _b.val); \
})
#endif
