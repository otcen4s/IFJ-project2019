/*
* Project           : IFJ19 Compiler
* File              : stack.h
* Author            : Adam Žitňanský
*/

#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdarg.h>
#include "scanner.h"
#include "enums.h"

#define STACK_ERROR -1
#define ALLOC_CHUNK 100

enum stack_type
{
    INTEGER_TYPE,
    SYMBOL_TYPE,
} stack_data_type;

//struct for expression parsing 
typedef struct Symbol
{
    unsigned data_type; //data type of 
    unsigned symbol; //determinates what kind of symbol it is
} Symbol;


typedef union {
        int integer;
        Symbol symbol; 
        //add other types here;
} t_stack_item;

typedef struct stack_struct
{
    long  top;
    long cap;
    enum stack_type type;  
	t_stack_item* items;
} t_stack;

                            
int stack_empty ( const t_stack* s );
int stack_full ( const t_stack* s );
t_stack_item stack_top ( const t_stack* s, int*err);
int stack_push ( t_stack* s, ...);
t_stack_item stack_pop ( t_stack* s, int *err);
t_stack * stack_create(const unsigned cap, const enum stack_type type);
void stack_free(t_stack* s);
Symbol stack_topmost_terminal(t_stack * s, int *err);

#endif

