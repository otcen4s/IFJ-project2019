

#include "stack.h"

int stack_init  (t_stack* s ) 
{

if(!s) return STACK_ERROR;

s->top = -1;
}

int stack_empty ( const t_stack* s ) 
{
	  return s->top == -1;
}

int stack_full ( const t_stack* s ) 
{
	return s->top == s->cap-1;
}

t_stack_item* stack_top ( const t_stack* s) 
{
	if(stack_empty(s)) return NULL;

    return &(s->items[s->top]);
}


t_stack_item* stack_pop ( t_stack* s ) {

	return  &(s->items[s->top]);
}


int stack_push ( t_stack* s, int item_symbol, int item_type) {

    if (stack_full(s))
    {
    //ak nieje dostatocna kapacita musime ju rozsirit
    s->cap += ALLOC_CHUNK;
    if(!(s->items = realloc(s->cap*sizeof(t_stack_item)))) return STACK_ERROR;
    }

    //samotne vlozenie prvku na zasobnik
    s->items[++s->top].item_type =item_type;
    s->items[s->top].item_symbol =item_symbol;
    return 0; 
}

