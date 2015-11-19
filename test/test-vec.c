#include "cbit/vec.h"
#include <stdio.h>
DECL_VEC(const char *, ccp);
DECL_VEC(int, int);

int main() {
    vec_storage_ccp stor = VEC_STORAGE_INITER(&stor, cpp);
    struct vec_ccp *vec = &stor.v;
    vec_free_storage_ccp(vec);
    VEC_STORAGE_INIT(&stor, ccp);
    for (int i = 0; i < 20; i++) {
        char *x;
        asprintf(&x, "el%d", i);
        vec_append_ccp(vec, x);
    }
    vec_remove_ccp(vec, 5, 4);
    vec_add_space_ccp(vec, 9, 2);
    for (int i = 9; i <= 10; i++)
        vec->els[i] = "TILT";
    vec_concat_ccp(vec, vec);
    VEC_FOREACH(vec, i, const char **c, ccp) {
        printf("%zd->%s\n", i, *c);
    }
    struct vec_ccp vec2 = vec_copy_ccp(vec);
    printf("copied length: %zd\n", vec2.length);
    vec_free_storage_ccp(vec);
    vec_free_storage_ccp(&vec2);
    struct vec_int vec3 = VEC_INITER;
    for (int i = 0; i < 10; i++)
        memset(vec_appendp_n_int(&vec3, 2), i, 2 * sizeof(int));
    printf("els[5] = %x\n", vec3.els[5]);
}

/*
expect-exit 0
expect-output<<
0->el0
1->el1
2->el2
3->el3
4->el4
5->el9
6->el10
7->el11
8->el12
9->TILT
10->TILT
11->el13
12->el14
13->el15
14->el16
15->el17
16->el18
17->el19
18->el0
19->el1
20->el2
21->el3
22->el4
23->el9
24->el10
25->el11
26->el12
27->TILT
28->TILT
29->el13
30->el14
31->el15
32->el16
33->el17
34->el18
35->el19
copied length: 36
els[5] = 2020202
>>
*/
