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
int str_realloc(tString *s);
int str_destroy(tString *s);
int str_cmp_keyword(tString *s, const char* keyword);

#endif