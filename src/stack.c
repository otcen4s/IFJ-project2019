#include "stack.h"

t_stack* stack_create  (const unsigned cap, const enum stack_type type) 
{
    t_stack* stack; 
       
    if((stack = malloc(sizeof(t_stack))) == NULL) return NULL; 
 
    stack->cap = cap;
    stack->top = -1;
    stack->type = type;

   if((stack->items = malloc(sizeof(t_stack_item)*cap)) == NULL) return NULL; 


    return stack;
}

int stack_empty ( const t_stack* s ) 
{
	  return s->top == -1;
}

int stack_full ( const t_stack* s ) 
{
	return s->top == s->cap-1;
}


t_stack_item stack_top ( const t_stack* s, int *err) 
{
	if(stack_empty(s)) 
    {
        *err= STACK_ERROR;
        t_stack_item  empty;
        empty.integer = STACK_ERROR;
        return empty;
    }

    return (s->items[s->top]);
}



t_stack_item stack_pop ( t_stack* s, int *err) {
    if(stack_empty(s)) 
    {
        *err= STACK_ERROR;
        t_stack_item  empty;
        empty.integer = STACK_ERROR;
        return empty; 
    }
    return s->items[s->top--];
}


int stack_push ( t_stack* s, ...) 
{
    //params list
    va_list ap;
    va_start(ap, s);

    if (stack_full(s))
    {

    //if cappacity is too small we need to resize it
    s->cap += ALLOC_CHUNK;
   if((s->items = realloc(s->items, s->cap*sizeof(t_stack_item))) == NULL)
   {
       return STACK_ERROR;
   }
    }

    //take value from argument based on type of stack
    switch ( s->type ) {
        case INTEGER_TYPE:
            s->items[++s->top].integer = (int) va_arg(ap,int);
            break;

        // add other types here
        default:
            break;
    }
    
    return 0;
}

void stack_free(t_stack* s)
{
    free(s->items);
    free(s);
}