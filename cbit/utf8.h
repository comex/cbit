#pragma once
#include "opt.h"
#include <stdint.h>
#include <string.h>

#if DECL_OPT_SUPPORTED || !defined(_OPT_UINT32_T)
#define _OPT_UINT32_T
DECL_OPT_MAYBE_ALWAYS(uint32_t, uint32_t, PRIMITIVE);
#endif


#if 0
/* returns some(char) or none, and modifies *ptrp. */
UNUSED_STATIC_INLINE
opt_uint32_t utf8_decode_one(const char **CBIT_RESTRICT ptrp, const char *end) {
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
#endif

inline opt_uint32_t
utf8_decode_one_slow(const char **CBIT_RESTRICT ptrp, const char *end) {
    const char *ptr = *ptrp;
    if (ptr == end)
        goto bad;
    uint8_t b1; b1 = (uint8_t) *ptr;
    if (!(b1 & 0x80)) {
        *ptrp = ptr + 1;
        return some_uint32_t(b1);
    }
    if (!(b1 & 0x40))
        goto bad;
    for (int size = 2, bits = 0x20; size <= 4; size++, bits >>= 1) {
        if (!(b1 & bits)) {
            if ((size_t) (end - ptr) < size)
                goto bad;
            uint32_t val = b1 & (bits - 1);
            for (int i = 1; i < size; i++) {
                uint8_t bi = (uint8_t) ptr[i];
                if ((bi & 0xc0) != 0x80)
                    goto bad;
                val = (val << 6) | (bi & 0x3f);
            }
            if (val > 0x10ffff || (val - 0xd800) < 0x800)
                goto bad;
            *ptrp = ptr + size;
            return some_uint32_t(val);
        }
    }
bad:
    return none_uint32_t();
}

inline opt_uint32_t
utf8_decode_one(const char **CBIT_RESTRICT ptrp, const char *end) {
    if (0)
        slow: return utf8_decode_one_slow(ptrp, end);
    const char *ptr = *ptrp;
    if (cbit_unlikely((size_t) (end - ptr) < 4))
        goto slow;
    uint32_t word;
    memcpy(&word, ptr, 4);
    if (cbit_likely(!(word & 0x80))) {
        *ptrp = ptr + 1;
        return some_uint32_t(word & 0xff);
    }
    uint8_t size = __builtin_clz(~word << 24) & 7;
    // size should be 2, 3, 4 but might not be
    uint64_t mask = (1ull << (8 * size)) - 0x100;
    uint64_t masked = word & mask;
    if (cbit_unlikely(!mask ||
                      (masked & 0xc0c0c0c0) != (0x80808080 & mask)))
        goto slow;
    uint8_t b1_bits = 7 - size;
    uint8_t b1_shift = 6 * (size - 1),
            b2_shift = (14 - b1_shift) & 31,
            b3_shift = 28 - b1_shift,
            b4_shift = 24;
    uint32_t b1x = word & ((1 << b1_bits) - 1),
             b2x = masked & 0x00003f00,
             b3x = masked & 0x003f0000,
             b4x = masked & 0x3f000000;
    uint32_t res = (b1x << b1_shift) | (b3x >> b3_shift) | (b4x >> b4_shift) |
                   ((b2x >> b2_shift) | (b2x << (32 - b2_shift))); // hopefully roll
    if (res > 0x10ffff || (res - 0xd800) < 0x800)
        goto slow;
    *ptrp = ptr + size;
    return some_uint32_t(res);

}

inline opt_uint32_t
utf8_decode_one_branchy(const char **CBIT_RESTRICT ptrp, const char *end) {
    if (0)
        slow: return utf8_decode_one_slow(ptrp, end);
    const char *ptr = *ptrp;
    if (cbit_unlikely((size_t) (end - ptr) < 4))
        goto slow;
    uint32_t word;
    memcpy(&word, ptr, 4);
    if (cbit_likely(!(word & 0x80))) {
        *ptrp = ptr + 1;
        return some_uint32_t(word & 0xff);
    }
    if (!(word & 0x40))
        goto slow;
    uint32_t val;
    int size;
    if (!(word & 0x20)) {
        size = 2;
        if ((word & 0x0000c000) != 0x00008000)
            goto slow;
        val = (word & 0x1f) << 6 | (word & 0x3f00) >> 8;
    } else if (!(word & 0x10)) {
        size = 3;
        if ((word & 0x00c0c000) != 0x00808000)
            goto slow;
        val = (word & 0x1f) << 12 | (word & 0x3f00) >> 2 |
              (word & 0x3f0000) >> 16;
    } else if (!(word & 0x8)) {
        size = 4;
        if ((word & 0xc0c0c000) != 0x80808000)
            goto slow;
        val = (word & 0x1f) << 18 | (word & 0x3f00) << 4 |
              (word & 0x3f0000) >> 10 | (word & 0x3f000000) >> 24;
    } else
        goto slow;
    if (val > 0x10ffff || (val - 0xd800) < 0x800)
        goto slow;
    *ptrp = ptr + size;
    return some_uint32_t(val);
}
