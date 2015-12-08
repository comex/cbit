#include "cbit/np/sfmt.h"
#include <stdio.h>
void p(str x) {
    printf("[%zu]<%s>\n", x.length, x.els);
    str_free_storage(&x);
}
int main() {
    p(sfmt("hello %s", 5));
    bool t = true; /* but true itself isn't... */
    p(sfmt("hello %d %d %s %s", 5, t, 5, t));
    p(sfmt("hi|%05s|%.2s|%5s|%-5s", "foo", S("foo"), str_copy(S("foo")), "foo"));
}

/*
expect-exit 0
expect-output<<
[7]<hello 5>
[16]<hello 5 1 5 true>
[23]<hi|00foo|fo|  foo|foo  >
>>
*/
