#pragma once
#include "vec.h"
DECL_VEC(char, char);
typedef struct vec_char str;

#define S(strlit) ((str) {.length = sizeof(strlit)-1, .capacity = 0, .els = (strlit) })

