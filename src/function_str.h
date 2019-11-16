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
    tString* param_array;
    unsigned len;
    unsigned array_size;
}tFunc_str;

int func_str_init(tFunc_str* param_array[]);
int func_str_clear(tString* param_array[]);
int func_str_insert(tString* param_array[]);

#endif