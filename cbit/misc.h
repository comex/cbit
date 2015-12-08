#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#pragma once

#ifdef NDEBUG
#define CBIT_DEBUG 0
#else
#define CBIT_DEBUG 1
#endif

#define cbit_dassert assert

#if CBIT_DEBUG
    #define cbit_serious_assert assert
#else
    #define cbit_serious_assert(x) \
        do { if (!(x)) abort(); } while(0)
#endif

#define CBIT_STATIC_ASSERT(what, msg) \
    extern char __cbit_static_assertion(char (*)[(what) ? 1 : -1])
#define CBIT_STATIC_ASSERT_EXPR(what, msg) \
    sizeof(char[(what) ? 1 : -1])

#ifdef __cplusplus
    #define CBIT_INLINE inline
    #define CBIT_ANCIENTC 0
    #if __cplusplus >= 201103L
        #define CBIT_HAVE_VARIADIC_MACROS 1
        #undef CBIT_STATIC_ASSERT
        #define CBIT_STATIC_ASSERT static_assert
    #endif
#else
    #if __STDC_VERSION__ >= 199901L
        #define CBIT_ANCIENTC 0
        #define CBIT_HAVE_VARIADIC_MACROS 1
        #define CBIT_INLINE inline
        #define CBIT_RESTRICT restrict
    #endif
    #if __STDC_VERSION__ >= 201112L
        #undef CBIT_STATIC_ASSERT
        #define CBIT_STATIC_ASSERT _Static_assert
    #endif
#endif

#ifndef CBIT_ANCIENTC
#define CBIT_ANCIENTC 1
#endif

#ifndef CBIT_HAVE_VARIADIC_MACROS
#define CBIT_HAVE_VARIADIC_MACROS 0
#endif

#ifndef CBIT_INLINE
#define CBIT_INLINE __inline
#endif

#ifndef CBIT_RESTRICT
#define CBIT_RESTRICT __restrict
#endif

#if __GNUC__ > 4 || \
    (__GNUC__ == 4 && __GNUC_MINOR__ >= 9) || \
    (__clang__ && __apple_build_version__ > 9999999 /* XXX */) || \
    (__clang__ && !__apple_build_version__ && \
        (__clang_major__ > 3 || \
         (__clang_major__ == 3 && __clang_minor__ > 9999 /* XXX */)))
    #define CBIT_AUTO_TYPE __auto_type
#elif __cplusplus >= 201103L
    #define CBIT_AUTO_TYPE auto
    #define CBIT_AUTO(x, y) \
        auto x = (y)
#endif
#ifdef CBIT_AUTO_TYPE
    #define CBIT_AUTO(x, y) \
        CBIT_AUTO_TYPE x = (y)
#else
    #define CBIT_AUTO(x, y) \
        __typeof((y)) x = (y)
#endif

#define UNUSED_STATIC_INLINE \
    __attribute__((unused)) static CBIT_INLINE
#define UNUSED_STATIC_FORCEINLINE \
    __attribute__((unused, always_inline)) static CBIT_INLINE

#define LET_LOOP__(expr, ctr) \
    if (0) \
       __done_##ctr: continue; \
    else if (0) \
       __break_##ctr: break; \
    else \
        for (expr; ;) \
            if (1) \
                goto __body_##ctr; \
            else \
                for (;;) \
                    if (1) \
                        goto __break_##ctr; \
                    else \
                        for (;;) \
                            if (1) \
                                goto __done_##ctr; \
                            else \
                                __body_##ctr:

#define LET_LOOP_(expr, ctr) LET_LOOP__(expr, ctr)
#define LET_LOOP(expr) LET_LOOP_(expr, __COUNTER__)

#define LET__(expr, ctr) \
    if (0) \
      __done_##ctr:; \
    else \
        for (expr; ;) \
            if (1) \
                goto __body_##ctr; \
            else \
                for (;;) \
                    if (1) \
                        goto __done_##ctr; \
                    else \
                        __body_##ctr:

#define LET_(expr, ctr) LET__(expr, ctr)
#define LET(expr) LET_(expr, __COUNTER__)

/* XXX */
#define safe_mul(a, b) ((a) * (b))
#define safe_add(a, b) ((a) + (b))

#define cbit_max(a, b) ((a) > (b) ? (a) : (b))
#define cbit_min(a, b) ((a) > (b) ? (b) : (a))

#if CBIT_HAVE_VARIADIC_MACROS
#define cbit_panic(...) do { \
    fprintf(stderr, __VA_ARGS__); \
    abort(); \
} while(0)
#else
#include <stdarg.h>
UNUSED_STATIC_INLINE
void cbit_panic(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    abort();
}
#endif

#define cbit_likely(x)   __builtin_expect(!!(x), 1)
#define cbit_unlikely(x) __builtin_expect(!!(x), 0)

#ifdef __identifier
#define CBIT_HAVE_MS_EXTS 1
#else
#define CBIT_HAVE_MS_EXTS 0
#endif

#if __GNUC__ && !__STRICT_ANSI__
#define CBIT_HAVE_GNU_EXTS 1
#else
#define CBIT_HAVE_GNU_EXTS 0
#endif
