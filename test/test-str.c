#include "cbit/str.h"
#include <stdio.h>

static void test_autofree() {
    int i;
    str dummy = STR_INITER;
    str_resize(&dummy, 1000000);
    for (i = 0; i < 100; i++) {
        #if CBIT_HAVE_GNU_EXTS
            autofree_str
        #else
            str
        #endif
        s;
        s = str_copy(&dummy);
        #if !CBIT_HAVE_GNU_EXTS
            str_free_storage(&s);
        #endif
    }
}

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
    test_autofree();
}
/*
expect-exit 0
expect-output<<
hello world
>>
*/
