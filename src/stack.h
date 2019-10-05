
#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#define STACK_ERROR -1
#define ALLOC_CHUNK 100;

// 
typedef struct stack_item 
{
    int item_symbol;
    int item_type; // typ polozky
} t_stack_item;

typedef struct
{
    unsigned long  top; //index vrcholu 
    unsigned long cap; //kapacita zasobnika
	t_stack_item* items; //pole prvkov         
} t_stack;

                                
int stack_init(t_stack* s);
int stack_empty ( const t_stack* s );
int stackFull ( const t_stack* s );
t_stack_item* stack_top ( const t_stack* s);
t_stack_item* stack_pop ( t_stack* s);
int stack_push ( t_stack* s, int item_symbol, int item_type);

#endif

