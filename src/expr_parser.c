#include "expr_parser.h"
//#define DEBUG

/******MACROS********/

//debug print 
#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

/******FORWARD DECLARATIONS********/
int token_enum_to_symb_enum (Token_type token);
int set_symbol_to_curr_token(Token token, Expr_parser* expr_parser);
Symbol crete_symbol(Symbol_enum symbol, Sym_data_type type);
int init_expr_parser(Expr_parser * expr_parser);
void dispose_expr_parser(Expr_parser * expr_p);
int reduce(Expr_parser * expr_parser);
int get_reduction_rule(Expr_parser* expr_parser);

/**
 * Parsing table to determine next parser action based on
 * currently readed symbol and topmost terminal symbol
 * on symbol stack
 **/
int parsing_table[15][15] =
{
//	  +  -  *  /  // > ==  != <= >= <  (  )  o  $ 
	{ R, R, S, S, S, R, R, R, R, R, R, S, R, S, R }, // +
    { R, R, S, S, S, R, R, R, R, R, R, S, R, S, R }, //-
	{ R, R, R, R, R, R, R, R, R, R, R, S, R, S, R }, // *
    { R, R, R, R, R, R, R, R, R, R, R, S, R, S, R }, // /
    { R, R, R, R, R, R, R, R, R, R, R, S, R, S, R }, // //
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // >
	{ S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // ==
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // !=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // <=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // >=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, R }, // <
	{ S, S, S, S, S, S, S, S, S, S, S, S, S, S, F }, // (
	{ R, R, R, R, R, R, R, R, R, R, R, F, R, F, R }, // )
	{ R, R, R, R, R, R, R, R, R, R, R, F, R, F, R }, // o operand 
	{ S, S, S, S, S, S, S, S, S, S, S, S, F, S, A }, // $
};


/**
    Helper function for creation of symbol struct
    @return return Symbol struct filled with given data
**/
Symbol create_symbol(Symbol_enum symbol, Sym_data_type type)
{
    Symbol sym;
    sym.data_type = type;
    sym.symbol = symbol;

    return sym;
}

/**
    Converts token type enum to symbol enum
    token type and symbol enums are equivalent until TOKEN_STRING,
    any other token is considered to be end of expr- $ 
    @return symbol enum quivalent to given token type
**/
int token_enum_to_symb_enum (Token_type token_enum)
{
    if(token_enum == TOKEN_STRING || token_enum == TOKEN_INTEGER 
       || token_enum == TOKEN_DECIMAL|| token_enum == TOKEN_IDENTIFIER
      ) return VALUE;

    //indicates end of expression
    if(token_enum > TOKEN_STRING) return DOLLAR;

    // here the enums matches each other, nothing to convert
    else return token_enum;
}

/**
    *Init of expr parser struct
    *@return NO_ERROR in case of success else err code 
**/
int init_expr_parser(Expr_parser * expr_parser)
{ 
    if (!expr_parser) return INTERNAL_ERROR;
    expr_parser->stack = stack_create(100,SYMBOL_TYPE);
    if (!expr_parser->stack) return INTERNAL_ERROR; 
    
    //push dollar to the stack to indicate stack end
    if(stack_push(expr_parser->stack, create_symbol(DOLLAR, SYM_UNDEF)))
    {
        return INTERNAL_ERROR;
    } 

    return NO_ERROR;
}

/**
 * Destructor for exrpr parser 
 **/
void dispose_expr_parser(Expr_parser * expr_p)
{ 
    stack_free(expr_p->stack);
}

/**
 * Sets expr_parser data based on last token
 **/

int  set_symbol_to_curr_token(Token token, Expr_parser* expr_parser)
{
    expr_parser->curr_sym.symbol = token_enum_to_symb_enum(token.type);
    expr_parser->curr_sym.data_type =token.type;
    return NO_ERROR; 
}

/**
 * Checks which rule should be now used for reduction 
 * only for binary operations
 * @return number of rule in rules enum
 **/
int get_reduction_rule(Expr_parser* expr_parser)
{
    //int err = NO_ERROR;

    /********* E -> E + E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == PLUS &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_PLUS_E;
    }

    /********* E -> E - E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == MINUS &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_MINUS_E;
    }

     /********* E -> E * E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == MUL &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_MUL_E;
    }

     /********* E -> E / E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == DIV &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_DIV_E;
    }

        
    /********* E -> E // E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == IDIV &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_IDIV_E;
    }

     /********* E -> E == E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == EQ &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_EQ_E;
    }

    /********* E -> E > E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == GTH &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_GTH_E;
    }

    /********* E -> E < E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == LTH &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_LTH_E;
    }

    /********* E -> E >= E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == GEQ &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_GEQ_E;
    }

    /********* E -> E <= E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == LEQ &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_LEQ_E;
    }

    /********* E-> (E) ***************/
    if( expr_parser->op1.symbol == RBRACKET &&
        expr_parser->op2.symbol == NON_TERM &&
        expr_parser->op3.symbol == LBRACKET)
    {
        return PAR_E_PAR;
    }

    //cant be reduced by any of the above rules
    return NO_RULE;    
}

/**
 * Reduces expression using production rules
 **/
int reduce(Expr_parser * expr_parser)
{
    int err = NO_ERROR;

    /********* SINGLE OPERAND E-> id ******************/
    if(expr_parser->stack_top_sym.symbol == VALUE)
    { 
        //pop replaced symbol from stack 
        Symbol replaced = stack_pop(expr_parser->stack, &err).symbol;
        if(err) return err;

        //change it to non terminal
        replaced.symbol = NON_TERM;
        stack_push(expr_parser->stack, replaced); 
        return NO_ERROR;       
    }

    /********* BINARY OPERATORS RULES E-> E OPERATOR E  ******************/
    
    //get operands from sym stack and check if there are correct op count 
    expr_parser->op1= stack_pop(expr_parser->stack, &err).symbol;
    if(err) return INTERNAL_ERROR;
    if(expr_parser->op1.symbol == DOLLAR) return SYNTAX_ERROR; // wrong operands count

    expr_parser->op2= stack_pop(expr_parser->stack, &err).symbol;
    if(err) return INTERNAL_ERROR;
    if(expr_parser->op2.symbol == DOLLAR) return SYNTAX_ERROR; // wrong operands count


    expr_parser->op3= stack_pop(expr_parser->stack, &err).symbol;
    if(err) return INTERNAL_ERROR;
    if(expr_parser->op3.symbol == DOLLAR) return SYNTAX_ERROR; // wrong operands count

    //determine which rule it is depending on operands
    int rule_to_apply = get_reduction_rule(expr_parser);
    if(rule_to_apply == NO_RULE) return SYNTAX_ERROR; //doesnt match any rule

    ///////////apply rule////////////
    switch (rule_to_apply)
    {
    case E_PLUS_E:
       gen_adds();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    case E_MINUS_E:
       gen_subs();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    case E_MUL_E:
       gen_muls();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_DIV_E:
       gen_divs();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_IDIV_E:
       gen_idivs();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_EQ_E:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_GTH_E:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_LTH_E:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_LEQ_E:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_GEQ_E:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    case PAR_E_PAR:
       //TODO generate stack compare here 
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    default:
        return INTERNAL_ERROR; 
    }
}

/////////////////////////////////////////////////////////////////////////////////

//expression parser interface 
int expression(Parser* parser)
{
    /**
     *creation and initialisation of expr_parser
     *expr_parser struct stores data needed for expression parsing
     **/
   
    Expr_parser e;
    Expr_parser * expr_parser = &e; 
    init_expr_parser(expr_parser);
    
    
    //proceessing whole expression token by token
    while(1)
    {        
        //set current symbol atributes in expr_parser based on currently readed token
        set_symbol_to_curr_token(parser->curr_token, expr_parser);
        DEBUG_PRINT("current readed sym %d \n",expr_parser->curr_sym.symbol);
        //get current stack top symbol
        int err = NO_ERROR;
        expr_parser->stack_top_sym = stack_topmost_terminal(expr_parser->stack, &err);
        DEBUG_PRINT("topmost terminal je %d \n", expr_parser->stack_top_sym.symbol, expr_parser->stack_top_sym.data_type);
        if(err)
        { 
            return INTERNAL_ERROR;
        }

         DEBUG_PRINT("start\n");
        
        //get expr parser action from parsing table based on curr symol and curr sym stack top
        Expr_parser_actions parser_action;
        parser_action = parsing_table[expr_parser->stack_top_sym.symbol][expr_parser->curr_sym.symbol];
        DEBUG_PRINT("parser action %d \n", parser_action);

        /***************** SHIFT *******************************/
        if(parser_action ==  S)
        {
            if(stack_push(expr_parser->stack, expr_parser->curr_sym))
            {
                return INTERNAL_ERROR;
            } 

            if (expr_parser->curr_sym.symbol == VALUE)
            {   
                //TODO check if all current id is deffined 

                //generate stack push instruction here
                gen_pushs(parser->curr_token, !parser->in_function);
                DEBUG_PRINT("pushing value to the stack \n");
            }            

            //get next token
            int err;
            parser->curr_token = read_token(parser->scanner, &err);
            if(err) return err;
        }

        /***************** REDUCE *********************************/
        if(parser_action == R)
        {
            int err;
            if((err= reduce(expr_parser)))
            {
                return err;
            }

            Symbol top= stack_top(expr_parser->stack, &err).symbol;
            DEBUG_PRINT("non term je  %d, %d \n" ,top.data_type, top.symbol ); 
        }

       /***************** ERROR ***********************************/
        if(parser_action == F)
        {
            dispose_expr_parser(expr_parser);
            DEBUG_PRINT("INVALID EXPRESSION \n");
            return SYNTAX_ERROR;
        }

        /***************** ACCEPT **********************************/
        if(parser_action == A )
        {
            DEBUG_PRINT("VALID EXPRESSION \n");
            //TODO move result into rhs here
            return NO_ERROR; 
        }     
    }
}

