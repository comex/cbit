#include "cbit/htab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
static bool ts_null(const char *const *ts) {
    if (0) printf("null? %p\n", *ts);
    return !*ts;
}
static bool ts_eq(const char *const *ts1, const char *const *ts2) {
    if (0) printf("eq? %p %p\n", *ts1, *ts2);
    return !strcmp(*ts1, *ts2);
}
#define ts_hash(strp) strlen(*(strp))
DECL_EXTERN_HTAB_KEY(teststr, const char *, ts_null, /*nil_byte*/ 0);
DECL_HTAB(teststr_int, teststr, int);

#define u32_hash(up) (*(up) % 100)
#define u32_null(up) (!*(up))
#define u32_eq(u1p, u2p) (*(u1p) == *(u2p))

DECL_STATIC_HTAB_KEY(u32, uint32_t, u32_hash, u32_eq, u32_null, 0);
DECL_HTAB(u32_u32, u32, uint32_t);

DECL_HTAB_KEY_ALIAS(intptr, int *, __htab_ptrbytes);

int main() {
    int i;
    struct htab_teststr_int *hp;
    HTAB_STORAGE(u32_u32) h2;
    uint32_t h2_raw[501];
    /* test loop crap */
#ifdef SUPER_OLD_FASHIONED
    HTAB_STORAGE(teststr_int) stor;
    int y;
    LET(y = 5)
#else
    LET(int y = 5)
#endif
        printf("5=%d\n", y);
    for (i = 0; ; i++) {
#ifdef SUPER_OLD_FASHIONED
        int x;
        LET_LOOP(x = 5)
#else
        LET_LOOP(int x = 5)
#endif
        {
            printf("*%d.%d\n", i, x);
            if (i == 4)
                break;
            else
                continue;
        }
        abort();
    }

#ifdef SUPER_OLD_FASHIONED
    HTAB_STORAGE_INIT(&stor, teststr_int);
#else
    HTAB_STORAGE(teststr_int) stor = HTAB_STORAGE_INITER(&stor, teststr_int);
#endif
    hp = &stor.h;
    for (i = 0; i < 100; i++) {
        const char *k;
        bool is_new;
        asprintf((char **) &k, "foo%d", i);
        *htab_setp_teststr_int(hp, &k, &is_new) = i;
        assert(is_new);
        assert(htab_getbucket_teststr_int(hp, &k)->value == i);
        assert(*htab_getp_teststr_int(hp, &k) == i);
    }
    {
        const char *k = "foo31";
        bool is_new;
        htab_setp_teststr_int(hp, &k, NULL);
        htab_setp_teststr_int(hp, &k, &is_new);
        assert(!is_new);
        htab_remove_teststr_int(hp, &k);
    }
#ifdef SUPER_OLD_FASHIONED
    /* TODO make some kind of 'iterator' macro? */
    {
        size_t b;
        for (b = 0; b < hp->capacity; b++) {
            if (ts_null(&hp->base[b].key))
                continue;
            if(hp->base[b].value % 10 != 1)
                continue;
            printf("%s -> %d\n",
                   hp->base[b].key,
                   hp->base[b].value);
        }
    }
#else
    HTAB_FOREACH(hp, const char **k, int *v, teststr_int) {
        if(*v % 10 == 1)
            printf("%s -> %d\n", *k, *v);
    }
#endif
    htab_free_storage_teststr_int(hp);

    HTAB_STORAGE_INIT(&h2, u32_u32);
    memset(h2_raw, 0xff, sizeof(h2_raw));
    for (i = 0; i < 3000; i++) {
        uint32_t op = arc4random() & 1;
        uint32_t key = (arc4random() % 500) + 1;
        if (op == 0) { /* set */
            uint32_t val = arc4random() & 0x7fffffff;
            *htab_setp_u32_u32(&h2.h, &key, NULL) = val;
            h2_raw[key] = val;
        } else { /* delete */
            htab_remove_u32_u32(&h2.h, &key);
            h2_raw[key] = -1;
        }
    }
    {
        uint32_t k;
        for (k = 1; k <= 500; k++) {
            uint32_t raw = h2_raw[k];
            uint32_t *hashedp = htab_getp_u32_u32(&h2.h, &k);
            uint32_t hashed = hashedp ? *hashedp : -1;
            /* printf("%d %x %x\n", k, raw, hashed); */
            assert(hashed == raw);
        }
    }
}

/*
expect-output<<
5=5
*0.5
*1.5
*2.5
*3.5
*4.5
foo1 -> 1
foo11 -> 11
foo21 -> 21
foo41 -> 41
foo51 -> 51
foo61 -> 61
foo71 -> 71
foo81 -> 81
foo91 -> 91
>>
expect-exit 0
*/

IMPL_HTAB_KEY(teststr, ts_hash, ts_eq);
