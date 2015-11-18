#pragma once
#include <stdatomic.h>

#define CREGEX_POSIX

#if CREGEX_POSIX
#include <regex.h>
#elif CREGEX_PCRE
#include <pcre.h>
#else
#error need CREGEX_POSIX || CREGEX_PCRE
#endif

#if CREGEX_POSIX
typedef regex_t cre;
#else
typedef pcre cre;
#endif

#define RE(pat, opts) ({ \
    static _Atomic(cre *) __cre; \
    cre *__ocre = atomic_load_explicit(&__cre, memory_order_consume); \
    if (!__ocre) \
        __ocre = cre_init_atomic(&__cre, pat, opts); \
    __ocre; \
})

static cre *cre_init_atomic(_Atomic(cre *) *crep, const char *pat, const char *opts) {
    cre *newcre = cre_mustcomp(pat, opts), *expected = NULL;
    if (atomic_compare_exchange_strong_explicit(crep, &expected, newcre,
                                                memory_order_release,
                                                memory_order_consume)) {
        return newcre;
    } else {
        cre_free(newcre);
        return expected;
    }
}

static cre *cre_mustcomp(const char *pat, const char *opts) {
    cre *ret = cre_comp(pat, opts);
    if (!ret)
        cbit_panic("cre_mustcomp: failed to compile '%s' opts='%s'\n", pat, opts);
    return ret;
}


