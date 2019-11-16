#include "expr_parser.h"

//forward declarations
int token_enum_to_symbol (Token_type token);
Symbol crete_symbol(Symbol_enum symbol, Sym_data_type type);
int init_expr_parser(Expr_parser * expr_parser);
void dispose_expr_parser(Expr_parser * expr_p);

//TODO needs a check
int parsing_table[14][14] =
{
//	  +  -  *  /  > ==  != <= >= <  (  )  o  $ 
	{ R, R, S, S, R, R, R, R, R, R, S, R, S, R }, // +
    { R, R, S, S, R, R, R, R, R, R, S, R, S, R }, //-
	{ R, R, R, R, R, R, R, R, R, R, S, R, S, R }, // *
    { R, R, R, R, R, R, R, R, R, R, S, R, S, R }, // /
	{ S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // ==
    { S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // !=
    { S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // <=
    { S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // >=
    { S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // <
    { S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // >
	{ S, S, S, S, S, S, S, S, S, S, S, E, S, F }, // (
	{ R, R, R, R, R, R, R, R, R, R, F, R, F, R }, // )
	{ R, R, R, R, R, R, R, R, R, R, F, R, F, R }, // o operand 
	{ S, S, S, S, S, S, S, S, S, S, S, F, S, F }, // $
};


//hepler function, returns symbol struct filled with given values
Symbol create_symbol(Symbol_enum symbol, Sym_data_type type)
{
    Symbol sym;
    sym.data_type = type;
    sym.symbol = symbol;

    return sym;
}

/*
    Converts token type enum to symbol enum
     token type and symbol enums are equivalent until TOKEN_STRING,
     any other token is considered to be end of expr- $ 
*/
int token_enum_to_symbol (Token_type token_enum)
{
    if(token_enum > TOKEN_STRING) return DOLLAR;
    else return token_enum;
}

int init_expr_parser(Expr_parser * expr_parser)
{ 
    if (!expr_parser) return INTERNAL_ERROR;

    expr_parser->stack = stack_create( SYMBOL_TYPE, 100);
    if (!expr_parser->stack) return INTERNAL_ERROR; 
    
    //push dollar to the stack to indicate stack end
    stack_push(expr_parser->stack, create_symbol(DOLLAR, SYM_UNDEF));

    expr_parser->valid_sym_cnt= 0;

    return NO_ERROR;
}

void dispose_expr_parser(Expr_parser * expr_p)
{ 
    stack_free(expr_p->stack);
}

/////////////////////////////////////////////////////////////////////////////////

//expression parser interface 
int expression(Parser* parser)
{
    Expr_parser expr_parser; 
    
    // expr parser struct to store data needed for parsing

    // iterate thru whole expression 

    /*
    while(true)
    {

    }
    */

   return NO_ERROR; 

}


