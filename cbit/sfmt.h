#pragma once

#include "str.h"
#include <stdbool.h>

#define SFMT_FIRST(arg, args...) arg
#define SFMT_IGNORE(...)
#define SFMT_FOURTH(args...) SFMT_FOURTH_(args)
#define SFMT_FOURTH_(a1, a2, a3, a4, args...) a4
/* following is not 100% right since it will get confused if you pass the
 * identifier for a function-like macro - maybe also if the argument expands to
 * contain commas
 * https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/ is nice and thorough
 * but really, just don't do that.
 */
#define SFMT_IF_EMPTY(arg, empty, nonempty) \
    SFMT_IF_EMPTY_(arg, empty, nonempty)
#define SFMT_IF_EMPTY_(arg, empty, nonempty) \
    SFMT_FOURTH(SFMT_COMMA arg SFMT_COMMA arg (), nonempty, empty, nonempty)
#define SFMT_COMMA(...) ,



#define SFMT_ARGS(args...) \
    SFMT_ARGS0(args)


/* this one is special, no comma */
#define SFMT_ARGS0(arg, args...) \
    SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS1)(args)

/*
for i in {1..31}; do echo "#define SFMT_ARGS$i(arg, args...) \\"; echo "    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS$((i+1)))(args)"; done
*/
#define SFMT_ARGS1(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS2)(args)
#define SFMT_ARGS2(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS3)(args)
#define SFMT_ARGS3(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS4)(args)
#define SFMT_ARGS4(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS5)(args)
#define SFMT_ARGS5(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS6)(args)
#define SFMT_ARGS6(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS7)(args)
#define SFMT_ARGS7(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS8)(args)
#define SFMT_ARGS8(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS9)(args)
#define SFMT_ARGS9(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS10)(args)
#define SFMT_ARGS10(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS11)(args)
#define SFMT_ARGS11(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS12)(args)
#define SFMT_ARGS12(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS13)(args)
#define SFMT_ARGS13(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS14)(args)
#define SFMT_ARGS14(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS15)(args)
#define SFMT_ARGS15(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS16)(args)
#define SFMT_ARGS16(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS17)(args)
#define SFMT_ARGS17(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS18)(args)
#define SFMT_ARGS18(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS19)(args)
#define SFMT_ARGS19(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS20)(args)
#define SFMT_ARGS20(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS21)(args)
#define SFMT_ARGS21(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS22)(args)
#define SFMT_ARGS22(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS23)(args)
#define SFMT_ARGS23(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS24)(args)
#define SFMT_ARGS24(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS25)(args)
#define SFMT_ARGS25(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS26)(args)
#define SFMT_ARGS26(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS27)(args)
#define SFMT_ARGS27(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS28)(args)
#define SFMT_ARGS28(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS29)(args)
#define SFMT_ARGS29(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS30)(args)
#define SFMT_ARGS30(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS31)(args)
#define SFMT_ARGS31(arg, args...) \
    , SFMT_ARG(arg) SFMT_IF_EMPTY(SFMT_FIRST(args), SFMT_IGNORE, SFMT_ARGS32)(args)

#define SFMT_ARGS32(...) \
    , _Static_assert(0, "too many arguments to SFMT :(")


enum sfmt_argtype {
    SFAT_END,
    SFAT_B,
    SFAT_UC, SFAT_SC, SFAT_C,
    SFAT_US, SFAT_S,
    SFAT_UI, SFAT_I,
    SFAT_UL, SFAT_L,
    SFAT_ULL, SFAT_LL,
    SFAT_F, SFAT_D,
    SFAT_CSTR,
    SFAT_STR,
    SFAT_STR_OWNED,
    SFAT_UNKNOWN
};

struct test { int a; };

#define SFMT_ARG(arg) \
    SFMT_ARGTYPE(arg), (arg)
#define SFMT_ARGTYPE(arg) \
    _Generic(\
        /* the statement expression is to promote from array types */ \
        ({ typeof(arg) __arg; \
           __arg + 0; /* if you get an error on this line, it means you passed a bad type to sfmt */ \
           __arg; }), \
        /* note - char, short, bool, and float will be promoted and must be
         * retrieved as int/double, but at least for bool, the original type is
         * useful */ \
                 bool:      SFAT_B, \
        unsigned char:      SFAT_UC, \
          signed char:      SFAT_SC, \
                 char:      SFAT_C, \
        unsigned short:     SFAT_US, \
                 short:     SFAT_S, \
        unsigned int:       SFAT_UI, \
                 int:       SFAT_I, \
        unsigned long:      SFAT_UL, \
                 long:      SFAT_L, \
        unsigned long long: SFAT_ULL, \
                 long long: SFAT_LL, \
                 float:     SFAT_F, \
                 double:    SFAT_D, \
        unsigned char *:    SFAT_CSTR, \
          signed char *:    SFAT_CSTR, \
                 char *:    SFAT_CSTR, \
                  str *:    SFAT_STR, \
                    str:    SFAT_STR_OWNED, \
                default:    SFAT_UNKNOwN \
    )

// test
#define str_sfmt(fmt, rest...) str_sfmt_impl(fmt, SFMT_ARGS(rest), SFAT_END)
str str_sfmt_impl(const char *fmt, ...);
