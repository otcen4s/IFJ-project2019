
#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdarg.h>
#include "scanner.h"
#define STACK_ERROR -1
#define ALLOC_CHUNK 100

enum stack_type
{
    INTEGER_TYPE,
    TOKEN_TYPE,
} stack_data_type;


typedef union {
        int integer;
        Token token; 
} t_stack_item;

typedef struct
{
    long  top;
    long cap;
    enum stack_type type;  
	t_stack_item* items;
} t_stack;

                            
int stack_empty ( const t_stack* s );
int stack_full ( const t_stack* s );
t_stack_item stack_top ( const t_stack* s, int*err);
int stack_push ( int *err, t_stack* s, ...);
t_stack_item stack_pop ( t_stack* s, int *err);
t_stack * stack_create(const unsigned cap, const enum stack_type type);
void stack_free(t_stack* s);

#endif

