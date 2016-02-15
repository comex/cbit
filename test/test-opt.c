#include "cbit/opt.h"
#if !CBIT_ANCIENTC
#include "cbit/utf8.h"
#endif

#if CBIT_HAVE_MS_EXTS
DECL_OPT(uint16_t, uint16_t, PRIMITIVE);
DECL_OPT(int *, intp, NONZERO);
#endif

#if !CBIT_HAVE_GNU_EXTS
#define opt_eq opt_eq_uint32_t
#endif

#if CBIT_HAVE_GNU_EXTS
static opt_intp test_try(int a) {
    int test = *TRY(intp, a ? some_intp(&a) : none_intp());
    uint16_t b = TRY(intp, a ? none_uint16_t() : some_uint16_t(5));
    return some_intp((void *) (uintptr_t) (test + b));
}
#endif

int main() {
    #define a cbit_serious_assert
#if CBIT_HAVE_GNU_EXTS
    a(!test_try(0).have);
    a(!test_try(1).have);
#endif
}

/*
expect-exit 0
expect-output<<
>>
*/
