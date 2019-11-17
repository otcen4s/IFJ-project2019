#ifndef EXPR_PARSER 
#define EXPR_PARSER

#include "scanner.h"
#include "enums.h"
#include "stack.h"
#include "parser.h"

//Symbol struct definition is in stack.h

//operations of shift-reduce parser
typedef enum{
	S, //SHIFT
	R, // REDUCE
	E, //EQ
	F, // FAILED
	A //ACCEPT
} Expr_parser_actions; 

typedef enum{
	SYM_ID,
	SYM_INT,
	SYM_DECIM,
	SYM_STRING,
	SYM_UNDEF,
} Sym_data_type;

typedef enum{
	E_PLUS_E,
	E_MINUS_E,
	E_MUL_E,
	E_DIV_E,

} Rules_enum; 

typedef struct
{ 
	int curr_symbol;
	t_stack * stack;
	Symbol stack_top_sym;
	Symbol curr_sym; 
	Symbol op1;
	Symbol op2;
	Symbol op3;
	int valid_sym_cnt;
} Expr_parser;


/*
interface of this module

function which process expresion and call code generator in proper order

@return NO_ERROR in case of sucess else ERR
*/
int expression (Parser *parser);


#endif