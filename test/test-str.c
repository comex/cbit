#include "cbit/str.h"
#include <stdio.h>

int main() {
    str h = Sv("hello"), w = Sv("world");
#if CBIT_ANCIENTC
    str_storage o;
    STR_STORAGE_INIT(&o);
#else
    str_storage o = STR_STORAGE_INITER(&o);
#endif
    str_concat(&o.v, &h);
#if __STDC_VERSION__ >= 199901L
    str_concat(&o.v, S(" "));
#else
    str_appendchar(&o.v, ' ');
#endif
    str_concat(&o.v, &w);
    printf("%s\n", o.v.els);
}
/*
expect-exit 0
expect-output<<
hello world
>>
*/
