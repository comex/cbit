#include "cbit/opt.h"
#if !CBIT_ANCIENTC
#include "cbit/utf8.h"
#endif

#if CBIT_HAVE_MS_EXTS
DECL_OPT(uint16_t, uint16_t, normal);
DECL_OPT(int *, intp, nonzero);
#endif

int main() {
#if !CBIT_ANCIENTC
    const char test[] = "hi\xc4\x80\xe4\xb8\x96\xe7\x95\x8c\xf0\x9f\x92\xa9";
    const char *p = test, *e = p + sizeof(test) - 1;
    assert(*e == 0);
    assert(utf8_decode_one(&p, e) == some_uint32_t('h'));
    assert(utf8_decode_one(&p, e) == some_uint32_t('i'));
    assert(utf8_decode_one(&p, e) == some_uint32_t(0x100));
    assert(utf8_decode_one(&p, e) == some_uint32_t(0x4e16));
    assert(utf8_decode_one(&p, e) == some_uint32_t(0x754c));
    assert(utf8_decode_one(&p, e) == some_uint32_t(0x754c));
    assert(p == e);
    assert(utf8_decode_one(&p, e) == none_uint32_t(0x1f4a9));
#endif
}

/*
expect-exit 0
expect-output<<
>>
*/
