,
#include "expr_parser.h"


//forward declarations
int token_enum_to_symbol (Token_type token);


//expression parser interface 
int expression(Parser* parser)
{
    t_stack * Stack;

    // iterate thru whole expression 
    while(true)
    {

    }

}

/*
    Converts token type enum to symbol enum
     token type and symbol enums are equivalent until TOKEN_STRING,
     any other token is considered to be end of expr- $ 
*/
int token_enum_to_symbol (Token_type token_enum)
{
    if(token > TOKEN_STRING) return DOLLAR;
    else return token;
}


//TODO
int prec_table[9][9] =
{
//	  +  -  *  /  r  (  )  i  $ 
	{ R, R, S, S, R, S, R, S, R }, // +
    { R, R, S, S, R, S, R, S, R }, //-
	{ R, R, R, R, R, S, R, S, R }, // *
    { R, R, R, R, R, S, R, S, R }, // /
	{ S, S, S, S, F, S, R, S, R }, // r  == <> <= < >= >
	{ S, S, S, S, S, S, E, S, F }, // (
	{ R, R, R, R, R, F, R, F, R }, // )
	{ R, R, R, R, R, F, R, F, R }, // i (id, int, double, string)
	{ S, S, S, S, S, S, F, S, F }, // $
};
