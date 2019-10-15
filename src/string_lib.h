#ifndef _STRING_LIB_H
#define _STRING_LIB_H

#define REALLOC_CONST 128
#define ALLOC_SIZE 64

typedef struct{
    char* str;
    unsigned len;
    unsigned arr_size;
} tString;


int str_init(tString *s);

int str_realloc(tString *s, unsigned new_arr_size);

int str_destroy(tString *s);

int str_cmp_keyword(tString *s, const char* keyword);

int str_insert_char(tString *s, const char new_char);

int str_copy_string(tString *s, const char* new_string);

#endif