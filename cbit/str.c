#include "str.h"

str str_fmt(const char *format, ...) {
    char *out;
    va_list ap;
    va_start(ap);
    int ret = vasprintf(&out, format, ap);
    va_end(ap);
    if (ret == -1)
        cbit_panic("str_fmt: out of memory");
    return (str) {.length = ret, .capacity = ret, .els = cstr};
}
