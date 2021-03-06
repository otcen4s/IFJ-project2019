/*
* Project           : IFJ19 Compiler
* File              : expr_parser.c
* Author            : Adam Žitňanský
*/

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
bool is_defined(Parser *parser, int token_cnt, bool *is_global); 

/**
 * Parsing table to determine next parser action based on
 * currently readed symbol and topmost terminal symbol
 * on symbol stack
 **/
int parsing_table[16][16] =
{
//	  +  -  *  /  // > ==  != <= >= <  (  )  v  id $ 
	{ R, R, S, S, S, R, R, R, R, R, R, S, R, S, S, R }, // +
    { R, R, S, S, S, R, R, R, R, R, R, S, R, S, S, R }, //-
	{ R, R, R, R, R, R, R, R, R, R, R, S, R, S, S, R }, // *
    { R, R, R, R, R, R, R, R, R, R, R, S, R, S, S, R }, // /
    { R, R, R, R, R, R, R, R, R, R, R, S, R, S, S, R }, // //
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // >
	{ S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // ==
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // !=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // <=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // >=
    { S, S, S, S, S, F, F, F, F, F, F, S, R, S, S, R }, // <
	{ S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, F }, // (
	{ R, R, R, R, R, R, R, R, R, R, R, F, R, F, F, R }, // )
	{ R, R, R, R, R, R, R, R, R, R, R, F, R, F, F, R }, // value v
    { R, R, R, R, R, R, R, R, R, R, R, F, R, F, F, R }, // id  
	{ S, S, S, S, S, S, S, S, S, S, S, S, F, S, S, A }, // $
};

/**
 * Check if identifier from last token was defined
 **/
bool is_defined(Parser* parser, int token_cnt, bool *is_global)
{
    int err= NO_ERROR;
    char* id_name;
    *is_global = false; 

    //set id name from proper token (because 2 tokens were pre readed)
    id_name = token_cnt==1?parser->previous_token.attribute.string.str:
                           parser->curr_token.attribute.string.str;
    
    //lookup in symtables
    tSymbol_item *curr_sym_loc = symtab_lookup(parser->local_table,id_name, &err);
    tSymbol_item *curr_sym_glob =  symtab_lookup(parser->global_table,id_name, &err); 

    //inside function
    if(parser->is_in_def)
    {
       if (curr_sym_loc && curr_sym_loc->symbol_state== SYMBOL_DEFINED && curr_sym_loc->symbol_type == SYMBOL_VAR)
       {
           *is_global = false; 
           return true;
       }
      
        if (curr_sym_glob && curr_sym_glob->symbol_state == SYMBOL_DEFINED && curr_sym_glob->symbol_type == SYMBOL_VAR)
        {
            *is_global= true;
            return true;
        }
        return false;
    }
   
    //outside function

    *is_global = true;
    if(curr_sym_glob && curr_sym_glob ->symbol_state == SYMBOL_DEFINED && curr_sym_glob->symbol_type == SYMBOL_VAR) 
        return true;

    return false;     
}
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
    if(token_enum == TOKEN_STRING || 
       token_enum == TOKEN_INTEGER ||
       token_enum == KEYWORD_NONE ||
       token_enum == TOKEN_DECIMAL)
      {
          return VALUE;
      } 

    if(token_enum == TOKEN_IDENTIFIER) return ID; 

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
        dispose_expr_parser(expr_parser);
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

    /********* E -> E != E ***************/
    if( expr_parser->op1.symbol == NON_TERM &&
        expr_parser->op2.symbol == NEQ &&
        expr_parser->op3.symbol == NON_TERM)
    {
        return E_NEQ_E;
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
    if(expr_parser->stack_top_sym.symbol == VALUE ||
       expr_parser->stack_top_sym.symbol == ID)
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
       gen_stack("adds");
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    case E_MINUS_E:
       gen_stack("subs");
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    case E_MUL_E:
       gen_stack("muls");
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_DIV_E:
       gen_stack("divs");
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_IDIV_E:
       gen_stack("idivs");
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_EQ_E:
       gen_eqs();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_GTH_E:
        gen_gts();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_LTH_E:
       gen_lts();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_LEQ_E:
       gen_ltes();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_GEQ_E:
       gen_gtes();
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;
    
    case E_NEQ_E:
        gen_neqs();
        //add new not terminal which represents result to the sym stack
        err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
        if(err) return INTERNAL_ERROR;
        break;

    case PAR_E_PAR:
       //add new not terminal which represents result to the sym stack
       err=stack_push(expr_parser->stack, create_symbol(NON_TERM, SYM_UNDEF));
       if(err) return INTERNAL_ERROR;
       break;

    default:
        return INTERNAL_ERROR; 
    }
    return NO_ERROR; 
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

    //first 2 token are alredy readed 
    //this var helps to determine if they were processed alredy
    int token_cnt= 0;    
    
    //proceessing whole expression token by token
    while(1)
    {
        token_cnt ++;
        //set current symbol atributes in expr_parser based on currently readed token
        //need to check which token we are on because 2 were prereaded and passed in args 
        if(token_cnt == 1)
        {
            set_symbol_to_curr_token(parser->previous_token, expr_parser);
        }
        else
        {
            set_symbol_to_curr_token(parser->curr_token, expr_parser);
        }
        
        DEBUG_PRINT("current readed sym %d \n",expr_parser->curr_sym.symbol);
        //get current stack top symbol
        int err = NO_ERROR;
        expr_parser->stack_top_sym = stack_topmost_terminal(expr_parser->stack, &err);
        DEBUG_PRINT("topmost terminal je %d \n", expr_parser->stack_top_sym.symbol, expr_parser->stack_top_sym.data_type);
        if(err)
        { 
            dispose_expr_parser(expr_parser);
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
                dispose_expr_parser(expr_parser);
                return INTERNAL_ERROR;
            }
                
            bool id_is_global= false; 

            if (expr_parser->curr_sym.symbol == VALUE ||
                expr_parser->curr_sym.symbol == ID )
                
            {   //if it's id, check if it was defined 
                if(expr_parser->curr_sym.symbol == ID)
                {
                   // check if inserted id is defined
                   //if so id_is_global will be set
                   if(!is_defined(parser, token_cnt, &id_is_global))
                   {
                    dispose_expr_parser(expr_parser);
                    return UNDEFINE_REDEFINE_ERROR; 
                   }
                }

                //generate stack push instruction here
                //we must check if we need to push current or previous token because it was pre readed
                if(token_cnt == 1)
                {
                    gen_pushs(parser->previous_token,id_is_global);
                }
                else
                {
                    gen_pushs(parser->curr_token, id_is_global);
                }
                
                DEBUG_PRINT("pushing value to the stack \n");
            }            

            //get next token but only if 2 prereded tokens were processed alredy
            if(token_cnt >= 2)
            {
                int err;
                parser->curr_token = read_token(parser->scanner, &err);
                if(err)
                {
                    dispose_expr_parser(expr_parser);
                    return err;
                } 
            }
            continue;
        }

        /***************** REDUCE *********************************/
        if(parser_action == R)
        {
            int err;
            if((err= reduce(expr_parser)))
            {
                dispose_expr_parser(expr_parser);
                return err;
            }
            continue;
        }

       /***************** ERROR ***********************************/
        if(parser_action == F)
        {
            dispose_expr_parser(expr_parser);
            return SYNTAX_ERROR;
            continue; 
        }

        /***************** ACCEPT **********************************/
        if(parser_action == A )
        {
            //expression was empty
            if(token_cnt== 1) return SYNTAX_ERROR; 

            DEBUG_PRINT("VALID EXPRESSION \n");
            
            //if there was any left hand value assign the result there 
            if(parser->left_side != NULL)
            {
                gen_pops(parser->left_side->key, !parser->is_in_def);
            }
            
            dispose_expr_parser(expr_parser); 
            return NO_ERROR; 
        }     
    }
}

