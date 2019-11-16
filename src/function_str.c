#include "function_str.h"

int func_str_init(tFunc_params* ptr)
{
    ptr->param = malloc(sizeof(tString) * ALLOC_CONST);
    if(ptr->param == NULL) return INTERNAL_ERROR;
    
    ptr->len = 0;
    ptr->array_size = ALLOC_CONST;

    return NO_ERROR;
}

int func_str_insert(tFunc_params* ptr, tString* param)
{
    if((param->len + 1) > ptr->array_size) // if the upcoming parameter would not fit into our allocated structure then we must realloc
    {
        if(func_str_realloc(ptr)) return INTERNAL_ERROR;
    }
    ptr->param[ptr->len] = *param;
    ptr->len++;

    return NO_ERROR;
}

int func_str_realloc(tFunc_params *ptr)
{
    ptr->param = realloc(ptr->param, ptr->len + ALLOC_CONST);
    
    if(ptr->param == NULL) return INTERNAL_ERROR;

    ptr->array_size = ptr->len + ALLOC_CONST;

    return NO_ERROR;
}

void func_str_clear(tFunc_params *ptr)
{
    ptr->len = 0;
    ptr->array_size = 0;
    free(ptr->param);
}
