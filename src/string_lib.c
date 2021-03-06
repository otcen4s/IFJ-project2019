/*
* Project           : IFJ19 Compiler
* File              : string_lib.c
* Author            : Matej Otčenáš
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_lib.h"
#include "error.h"

int str_init(tString *s){
    s->str = (char*) malloc(ALLOC_SIZE * sizeof(char));
    if(s->str == NULL){
        return INTERNAL_ERROR; //99
    }
    s->len = 0;
    s->arr_size = ALLOC_SIZE;
    s->str[s->len] = '\0';

    return NO_ERROR; //0
}

int str_realloc(tString *s, unsigned new_arr_size){

    s->str = (char*)realloc(s->str, s->len + REALLOC_CONST + new_arr_size);

    if(s->str == NULL) return INTERNAL_ERROR;
    
    s->arr_size = new_arr_size + REALLOC_CONST + s->len;

    //realocated memory is uninitialised so we must set it to 0
    for(unsigned i=s->len; i < s->arr_size; i++)
    {
        s->str[i]= '\0';
    }

    return NO_ERROR;
}

void str_destroy(tString *s){
    s->len = 0;
    s->arr_size = 0;
    free(s->str);
}

int str_cmp_keyword(tString *s, const char* keyword){
    int value = strcmp(s->str, keyword);
    return !value;
}

int str_copy(tString *s, const char* new_string){
    unsigned new_str_len = strlen(new_string); // getting new size for reallocation, strlen() returns length of string without '/0' char counted

    if(new_str_len >= s->arr_size)
    {
        if(str_realloc(s, new_str_len) == INTERNAL_ERROR){
            return INTERNAL_ERROR;
        }
    }

    strcpy(s->str, new_string);
    s->len = new_str_len; //modified

    return NO_ERROR;
}

int str_insert_char(tString *s, const char new_char){
    
    if((s->len + 1) >= s->arr_size)
    {
        if(str_realloc(s, 1) == INTERNAL_ERROR)
        {
            return INTERNAL_ERROR;
        }
    }

    s->str[s->len] = new_char;
    s->str[s->len + 1] = '\0';
    s->len++;

    return NO_ERROR;
}

int str_append(tString *s, const char *new_string) {
    unsigned new_str_len = strlen(new_string);

    unsigned new_len = s->len + new_str_len;

    if(s->arr_size <= new_len)
    {
        if (str_realloc(s, new_len) == INTERNAL_ERROR) 
        {
            return INTERNAL_ERROR;
        }
    }

    strcat(s->str, new_string);
    s->len = new_len;

    return NO_ERROR;
}

int str_concat(tString *s, ...) {
    va_list ap;
    char *temp;

    str_copy(s, "");

    va_start(ap, *s);

    while ((temp = va_arg(ap, char*))) {
        str_append(s, temp);
    }

    va_end(ap);

    return NO_ERROR;

}