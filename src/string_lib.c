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
    s->str = (char*)realloc(s->str, REALLOC_CONST + new_arr_size); //fix

    if(s->str == NULL) return INTERNAL_ERROR;
    
    s->arr_size = new_arr_size + REALLOC_CONST;

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

    if(str_realloc(s, new_str_len) == INTERNAL_ERROR){
        return INTERNAL_ERROR;
    }

    strcpy(s->str, new_string);
    s->len = new_str_len + 1; //modified

    return NO_ERROR;
}

int str_insert_char(tString *s, const char new_char){
    if(str_realloc(s, 1) == INTERNAL_ERROR){
        return INTERNAL_ERROR;
    }
    s->str[s->len] = new_char;
    s->str[s->len + 1] = '\0';
    s->len++;

    return NO_ERROR;
}

