#ifndef _FUNCTION_STR_H_
#define _FUNCTION_STR_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_lib.h"
#include "error.h"


#define ALLOC_CONST 64

typedef struct
{
    tString* param;
    unsigned len;
    unsigned array_size;
}tFunc_params;

int func_str_init(tFunc_params* p);
void func_str_clear(tFunc_params* p);
int func_str_insert(tFunc_params* p, tString* param);
int func_str_realloc(tFunc_params* p);
#endif