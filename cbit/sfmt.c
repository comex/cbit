#include "sfmt.h"

struct sfmt_arg_internal {
    enum { SAI_ULL, SAI_DOUBLE, SAI_STR } type;
    bool ull_is_signed;
    bool ull_is_bool;
    bool str_is_owned;
    str str_storage;
    union {
        unsigned long long ull;
        double d;
        str *str;
    };
};

static void read_sfmt_arg_internal(struct sfmt_arg_internal *ai, va_list ap,
                                   const char *fmt) {
    ai->ull_is_bool = ai->ull_is_signed = ai->str_is_owned = false;
    int argtype = va_arg(ap, int);
    switch (argtype) {
    case SFAT_END:
        cbit_panic("not enough arguments for format '%s'\n", fmt);
        return;
    case SFAT_SC:
    case SFAT_S:
    case SFAT_I:
        ai->ull_is_signed = true;
        goto read_int;
    case SFAT_B:
        ai->ull_is_bool = true;
    case SFAT_C:
        ai->ull_is_signed = ((char) -1) < ((char) 0);
        goto read_int;
    case SFAT_UC:
    case SFAT_US:
    read_int:
        ai->type = SAI_ULL;
        ai->ull = (unsigned long long) va_arg(ap, int);
        break;
    #define COMMON_CASE(en, is_signed, ty) \
        case en: \
            ai->type = SAI_ULL; \
            ai->ull_is_signed = is_signed; \
            ai->ull = (unsigned long long) va_arg(ap, ty)

    COMMON_CASE(SFAT_UI,  false, unsigned int);
    COMMON_CASE(SFAT_UL,  false, unsigned long);
    COMMON_CASE(SFAT_L,   true, long);
    COMMON_CASE(SFAT_ULL, false, unsigned long);
    COMMON_CASE(SFAT_LL,  true, long long);

    #undef COMMON_CASE
    case SFAT_F:
    case SFAT_D:
        ai->type = SAI_DOUBLE;
        ai->d = va_arg(ap, double);
        break;
    case SFAT_CSTR:
        ai->type = SAI_STR;
        ai->str_storage = str_borrowc(va_arg(ap, char *));
        ai->str = &ai->str_storage;
        ai->str_is_owned = false;
        break;
    case SFAT_STR:
        ai->type = SAI_STR;
        ai->str = va_arg(ap, str *);
        ai->str_is_owned = false;
        break;
    case SFAT_STR_OWNED:
        ai->type = SAI_STR;
        ai->str_storage = va_arg(ap, str);
        ai->str = &ai->str_storage;
        ai->str_is_owned = true;
        break;
    default:
        cbit_panic("unknown SFAT %d - arguments mixed up?\n", argtype);
    }

}

#define TMP_LEN 65

static void sfmt_hex(char *tmp, str *out_str, unsigned long long ull,
                     bool is_signed, char poschar, bool altform, bool caps) {
    /* vector gather would make this cutely efficient, but it's pointless... */
    const char *lookup = caps ? "0123456789ABCDEF"
                              : "0123456789abcdef";
    char *ep = tmp + TMP_LEN, *ptr = ep;
    unsigned long long negmin = 1ull << (8 * sizeof(negmin) - 1);
    bool negative = is_signed && ull >= negmin;
    if (negative)
        ull = -ull;
    while (ull) {
        *--ptr = lookup[ull & 0xf];
        ull >>= 4;
    }
    if (altform) {
        *--ptr = caps ? 'X' : 'x';
        *--ptr = '0';
    }
    if (negative)
        *--ptr = '-';
    else if (poschar)
        *--ptr = poschar;
    *out_str = str_borrow(ptr, ep - ptr); 
}

static void sfmt_decint(char *tmp, str *out_str, unsigned long long ull,
                        bool is_signed, char poschar) {
    char *ep = tmp + TMP_LEN, *ptr = ep;
    unsigned long long negmin = 1ull << (8 * sizeof(negmin) - 1);
    bool negative = is_signed && ull >= negmin;
    if (negative)
        ull = -ull;
    while (ull) {
        *--ptr = '0' + (ull % 10);
        ull /= 10;
    }
    if (negative)
        *--ptr = '-';
    else if (poschar)
        *--ptr = poschar;
    *out_str = str_borrow(ptr, ep - ptr);
}

static void sfmt_decfloat(char *tmp, str *out_str, double num, char poschar,
                          bool altform, unsigned precision) {
    char *ptr = tmp + 1;
    int ret = snprintf(ptr, TMP_LEN - 1, altform ? "%#.*f" : "%.*f",
                       (int) precision, num);
    size_t size = cbit_min(ret, TMP_LEN - 1);
    if (num >= 0 && poschar) {
        *--ptr = num;
        size++;
    }
    *out_str = str_borrow(ptr, size);
}

str str_sfmt_impl(const char *fmt, ...) {
    str outstr = STR_INITER;
    str *to_print;
    bool free_to_print = false;
    va_list ap;
    va_start(ap, fmt);
    struct sfmt_arg_internal ai;
    char tmp[TMP_LEN];
    while (1) {
        char c = *fmt++;
        if (c == '\0')
            break;
        if (c != '%') {
            str_appendchar(&outstr, c);
            continue;
        }
        /*
            1. %
            2. [#-+ 0']*
            3. vec separator character (OS X ext, ignore)
            4. [0-9]*|\*([0-9]+\$))?  field width (ignore explicit position)
            5. \.<same-as-field-width>?  precision
            6. length (ignore)
            7. format char
        */
        bool altform = false,
             negate = false,
             caps = true;
        char poschar = 0;
        char padchar = ' ';
        unsigned width = 0;
        size_t precision = SIZE_MAX;
        while (1) {
            /* 2. */
            switch (c) {
            case '#':
                altform = true;
                break;
            case '-':
                negate = true;
                break;
            case ' ':
            case '+':
                poschar = c;
                break;
            case '0':
                padchar = '0';
                break;
            case '\'':
                goto unsupported;
            default:
                goto after_flags;
            }
            c = *fmt++;
        }
    after_flags:
        /* 4., 5. */
        for (int i = 0; ; i++) {
            unsigned out = 0;
            if ('0' <= c && c <= '9') {
                do {
                    out = (out * 10) + (c - '0');
                    c = *fmt++;
                } while ('0' <= c && c <= '9');
            } else if (c == '*') {
                c = *fmt++;
                if ('0' <= c && c <= '9') {
                    /* explicit position */
                    goto unsupported;
                }
                read_sfmt_arg_internal(&ai, ap, fmt);
                if (ai.type != SAI_ULL)
                    cbit_panic("wrong type for '*' argument in format string: %s\n", fmt);
                long long xout = (long long) ai.ull;
                if (xout < 0) {
                    if (!ai.ull_is_signed)
                        cbit_panic("'*' value out of range lol\n");
                    negate = true;
                    xout = -xout;
                }
                if (xout > 1000)
                    cbit_panic("'*' value out of range lol\n");
                out = (unsigned) xout;
            }
            if (i == 0) {
                width = out;
                if (c == '.')
                    c = *fmt++;
                else
                    break;
            } else {
                precision = out;
                break;
            }
        }
        /* 7. */
        read_sfmt_arg_internal(&ai, ap, fmt);
        to_print = &ai.str_storage;
        free_to_print = false;
        switch (c) {
        case 'p':
            altform = true;
            goto hex;
        case 'P':
            altform = true;
        case 'X':
            caps = true;
            goto hex;
        case 'x':
        hex:
            if (ai.type != SAI_ULL)
                cbit_panic("%%%c can only take arguments of integer type\n", c);
            sfmt_hex(tmp, &ai.str_storage, ai.ull, ai.ull_is_signed,
                     poschar, altform, caps);
            to_print = &ai.str_storage;
            break;
        case 'd':
            if (ai.type != SAI_ULL)
                cbit_panic("%%%c can only take arguments of integer type\n", c);
        case 's':
            switch (ai.type) {
            case SAI_ULL:
                if (altform)
                    cbit_panic("pointless alternate form on decimal in format string: %s\n", fmt);
                if (ai.ull_is_bool && c != 'd')
                    to_print = ai.ull ? S("true") : S("false")
                else
                    sfmt_decint(tmp, to_print, ai.ull, ai.ull_is_signed, poschar);
                break;
            case SAI_DOUBLE:
                sfmt_decfloat(tmp, to_print, ai.d, poschar, altform, precision);
                break;
            case SAI_STR:
                to_print = ai.str;
                free_to_print = ai.str_is_owned;
                break;
            }
        default:
            cbit_panic("unknown format character '%c' in format string: %s\n", c, fmt);
        }
        size_t actual_length = cbit_min(precision, to_print->length);
        size_t pad_length = actual_length >= width ? 0 : (width - actual_length);
        char *op = str_appendp_n(&outstr, pad_length + actual_length);
        if (pad_length)
            memset(op, padchar, pad_length);
        memcpy(op + pad_length, to_print->els, actual_length);

        if (free_to_print)
            str_free_storage(to_print);
    }
    va_end(ap);
}


int main() {
    str_sfmt("fmt", 2, 2ul, (short)2000, "lol", 42.0, 42.0f, (char)'f');
}
