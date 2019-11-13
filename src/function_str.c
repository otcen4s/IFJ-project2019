#include "function_str.h"

int func_str_init(tFunc_str* ptr)
{
    ptr->param_array = malloc(sizeof(tString) * ALLOC_CONST);
    if(ptr->param_array == NULL) return INTERNAL_ERROR;
    
    ptr->len = 0;
    ptr->array_size = ALLOC_CONST;

    return NO_ERROR;
}

int func_str_insert(tFunc_str* ptr, tString* param)
{
    ptr->param_array[ptr->len] = *param;
}