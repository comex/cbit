#include "cbit/opt.h"
#if !CBIT_ANCIENTC
#include "cbit/utf8.h"
#endif

#if CBIT_HAVE_MS_EXTS
DECL_OPT(uint16_t, uint16_t, primitive);
DECL_OPT(int *, intp, nonzero);
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
#if !CBIT_ANCIENTC
    const char test[] = "hi\xc4\x80\xe4\xb8\x96\xe7\x95\x8c\xf0\x9f\x92\xa9";
    const char *p = test, *e = p + sizeof(test) - 1;
    #define a cbit_serious_assert
    a(*e == 0);
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t('h')));
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t('i')));
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t(0x100)));
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t(0x4e16)));
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t(0x754c)));
    a(opt_eq(utf8_decode_one(&p, e), some_uint32_t(0x1f4a9)));
    a(p == e);
    a(opt_eq(utf8_decode_one(&p, e), none_uint32_t()));
#endif
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
