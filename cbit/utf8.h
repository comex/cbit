#pragma once
#include "opt.h"
#include <stdint.h>

#if DECL_OPT_SUPPORTED || !defined(_OPT_UINT32_T)
#define _OPT_UINT32_T
DECL_OPT_MAYBE_ALWAYS(uint32_t, uint32_t, normal);
#endif


/* returns some(char) or none, and modifies *ptrp. */
UNUSED_STATIC_INLINE
opt_uint32_t utf8_decode_one(const char **ptrp, const char *end) {
    const char *ptr = *ptrp;
    if (cbit_unlikely(ptr == end))
        return none_uint32_t();
    uint8_t b1 = (uint8_t) *ptr++;
    *ptrp = ptr;
    if (cbit_likely(!(b1 & 0x80)))
        return some_uint32_t(b1);
    uint8_t size = __builtin_clz((uint32_t) ~b1 << 24);
    size_t avail = (size_t) (end - *ptr);
    avail = cbit_min(avail, 4);
    if (cbit_unlikely(size > avail))
        return none_uint32_t();
    uint32_t chr = b1 & (0xff >> size);
    uint8_t b2 = (uint8_t) *ptr++;
    uint8_t _and = b2, _or = b2;
    chr = (chr << 6) | (b2 & 0x3f);
    if (size >= 3) {
        uint8_t b3 = (uint8_t) *ptr++;
        _and &= b3;
        _or |= b3;
        chr = (chr << 6) | (b3 & 0x3f);
        if (size == 4) {
            uint8_t b4 = (uint8_t) *ptr++;
            _and &= b4;
            _or |= b4;
            chr = (chr << 6) | (b4 & 0x3f);
        }
    }
    *ptrp = ptr;
    if (!(_and & 0x80) || (_or & 0x40))
        return none_uint32_t();
    return some_uint32_t(chr);
}
