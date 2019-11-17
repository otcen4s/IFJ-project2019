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

    Symbol sym; 

    //take value from argument based on type of stack
    switch ( s->type ) {
        case INTEGER_TYPE:
            s->items[++s->top].integer = (int) va_arg(ap,int);
            break;

        case SYMBOL_TYPE:
            sym = (Symbol) va_arg(ap,Symbol);
            s->items[++s->top].symbol = (Symbol) va_arg(ap,Symbol);
            break; 

        default:
            return INTERNAL_ERROR;
    }
    
    return NO_ERROR;
}

void stack_free(t_stack* s)
{
    free(s->items);
    free(s);
}


////////////////// FUNCTIONS USED FOR EXPR PARSING ////////////////////

Symbol stack_topmost_terminal(t_stack * s, int *err)
{
    *err = NO_ERROR; 

    if ( stack_empty(s))
    {
        *err= INTERNAL_ERROR;
        return;
    } 

    for (int i=s->top; i>= 0; i--)
    {
        if(s->items[i].symbol.symbol < NON_TERM) return s->items[i].symbol;
    }

    //terminal not found means internall error
    err = INTERNAL_ERROR; 
    return; 
}

/*

int stack_insert_after_topmost_term(t_stack* stack, Symbol sym)
{
    
}
*/