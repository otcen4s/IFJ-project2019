#ifndef EXPR_PARSER 
#define EXPR_PARSER

#include "scanner.h"
#include "enums.h"
#include "stack.h"
#include  "parser.h"

//operations of shift-reduce parser
typedef enum{
	S,
	R,
	E,
	F
} Expr_parser_actions; 


/*
interface of this module

function which process expresion and call code generator in proper order

@return NO_ERROR in case of sucess else ERR
*/
int expression (Parser *parser);


#endif