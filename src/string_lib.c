#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_lib.h"
#include "error.h"

str_init(tString *s){
    s->str = (char*) malloc(ALLOC_SIZE * sizeof(char));
    if(s->str == NULL){
        return INTERNAL_ERROR; //99
    }
    s->len = 0;
    s->arr_size = ALLOC_SIZE;
    s->str[s->len] = '\0';

    return NO_ERROR;
}

str_realloc(tString *s){
    s->str = realloc(s->str, REALLOC_CONST + s->arr_size);
    if(s->str == NULL) return INTERNAL_ERROR;
    s->arr_size += REALLOC_CONST;

    return NO_ERROR;
}

str_destroy(tString *s){
    s->len = 0;
    s->arr_size = 0;
    free(s->str);
}

str_cmp_keyword(tString *s, const char* keyword){
    int value = strcmp(s->str, keyword);
    return value;
}