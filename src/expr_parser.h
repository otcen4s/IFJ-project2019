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
	F, // FAILED
	A //ACCEPT
} Expr_parser_actions; 

typedef enum{
	VAL,
	SYM_UNDEF,
} Sym_data_type;


typedef enum{
	E_PLUS_E,
	E_MINUS_E,
	E_MUL_E,
	E_DIV_E,
	E_EQ_E,
	E_GTH_E,
	E_LTH_E,
	E_GEQ_E,
	E_LEQ_E,
	PAR_E_PAR,
	NO_RULE,

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