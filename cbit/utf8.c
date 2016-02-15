#include "utf8.h"

extern inline opt_uint32_t
utf8_decode_one_slow(const char **CBIT_RESTRICT ptrp, const char *end);

extern inline opt_uint32_t
utf8_decode_one(const char **CBIT_RESTRICT ptrp, const char *end);

extern inline opt_uint32_t
utf8_decode_one_branchy(const char **CBIT_RESTRICT ptrp, const char *end);
