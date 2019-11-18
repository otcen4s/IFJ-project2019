#include "parser.h"

/************** MACROS **********/
#define CHECK_ERROR() if(err) return err

#define GET_NEXT_TOKEN()                                                            \
                        do{                                                         \
                            parser->curr_token = read_token(parser->scanner, &err); \
                            CHECK_ERROR();                                          \
                        }while(0)

#define CHECK_TOKEN(token)                                                              \
                        do{                                                             \
                            if(parser->curr_token.type != (token)) err = SYNTAX_ERROR;  \
                            else err = NO_ERROR;                                        \
                        }while(0)

#define GET_KEY() tString key = parser->curr_token.attribute.string

#define ADD_BUILT_IN_FUNC(id,type)                                                          \
                                do{                                                         \
                                    tSymbol_item* tmp;                                      \
                                    tmp = symtab_add(parser->global_table, (id), &err);     \
                                    CHECK_ERROR();                                          \
                                    tmp->data_type = (type);                                \
                                    tmp->symbol_state = SYMBOL_DEFINED;                     \
                                    tmp->symbol_type = SYMBOL_FUNC;                         \
                                    /*tmp->symbol_validity = SYMBOL_BUILT_IN;  */               \
                                }while(0)

/********* PREDECLARATIONS ***********/
int statement (Parser *parser);
int statement_inside (Parser *parser);
int params (Parser *parser);
int next_params (Parser *parser);
int expression_start (Parser *parser);
int expression_next (Parser *parser);
int value (Parser *parser);
int arg (Parser *parser);

/********* GLOBAL VARIABELS ***********/
int counter_var = 1;
bool is_if = false;
bool is_while = false;
bool is_def = false;
unsigned nested_cnt = 0;


/****************** FUNCTION DEFINITIONS ************************/

int init_parser(Parser* parser)
{
    if(((parser->scanner = malloc(sizeof(Scanner))) == NULL) || (symtab_init(parser->global_table)) || (symtab_init(parser->local_table))) return INTERNAL_ERROR;

    int err;
    parser->curr_token.type = -1;
    parser->symbol_data = NULL;

    /* Predefine built-in functions into global table*/
    ADD_BUILT_IN_FUNC("print",DATA_NONE);
    ADD_BUILT_IN_FUNC("len",DATA_INT);
    ADD_BUILT_IN_FUNC("substr",DATA_STRING);
    ADD_BUILT_IN_FUNC("ord",DATA_INT);
    ADD_BUILT_IN_FUNC("chr",DATA_STRING);
    ADD_BUILT_IN_FUNC("inputs",DATA_STRING);
    ADD_BUILT_IN_FUNC("inputf",DATA_DOUBLE);
    ADD_BUILT_IN_FUNC("inputi",DATA_INT);
    
    return NO_ERROR;
}

void dispose_parser(Parser* parser)
{
    free(parser->scanner);
    symtab_free(parser->global_table);
    symtab_free(parser->local_table);
}

void generate_variable_key(tString* var)
{
    //clear
    var->str[0] = '\0';
    var->len = 0;
    str_insert_char(var, '$');

    for(int i = counter_var; i != 0;)
    {
        str_insert_char(var, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counter_var++;
}


// this function  will be the interface of parser (e.i. will be called from outside) 
int start_compiler(char* src_file_name)
{
    //create and initialise parser data structure 
    Parser p;
    Parser *parser = &p; 
    if(init_parser(parser))  return INTERNAL_ERROR;

    //initialisation of scanner 
    if(init_scanner(parser->scanner, src_file_name)) return INTERNAL_ERROR;

    //initialisation of global and local table
    if(symtab_init(parser->global_table)||
       symtab_init(parser->local_table))
    {
        return INTERNAL_ERROR;
    }

    //reading first token 
    int err;
    parser->curr_token = read_token(parser->scanner, &err);
    
    //in case there is lexical errror at the very beginning of the src file
    if(err) return err;

    //start syntactic analysis here e.i. call function which implements <program> rule 

    //temporary
    return NO_ERROR;    
}


/************************************ RULES ***********************************************/

/** Rule 1. <statement> -> EOF
 *  Rule 2. <statement> -> EOL <statement>
 *  Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> DEDENT <statement>
 *  Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement>
 *  Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> <end> DEDENT <statement> 
 *  Rule 6. <statement> -> ID = <expression_start> <end> <statement>
 *  Rule 7. <statement> -> PASS <end> <statement>
 *  Rule 8. <statement> -> PRINT ( <arg> ) <end> <statement>
*/
int statement(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();

    /* Rule 1. <statement> -> EOL */
    if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
    
    /* Rule 2. <statement> -> EOF <statement>*/
    if(parser->curr_token.type == TOKEN_EOL) 
    {
        err = statement(parser);
        CHECK_ERROR();
    }

    /* Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> DEDENT <statement> */
    if(parser->curr_token.type == KEYWORD_DEF)
    {
        is_def = true; // we are in the definition of function

        GET_NEXT_TOKEN(); // ID
        /* STATE: DEF ID */
        CHECK_TOKEN(TOKEN_IDENTIFIER); // check if token type is ID
        CHECK_ERROR(); //always check error 
       
        GET_KEY(); // Identifier is a special key in global or local symtable
        
        parser->symbol_data = symtab_add(parser->global_table, key.str, &err); // add ID of function into global table
        CHECK_ERROR(); // check for internal error of used function
        parser->symbol_data->symbol_type = SYMBOL_FUNC; // specifying it's function
        parser->symbol_data->symbol_state = SYMBOL_DEFINED; // -||- defined
        //parser->symbol_data->symbol_validity = SYMBOL_GLOBAL;


        GET_NEXT_TOKEN();
        /* STATE: DEF ID ( */
        CHECK_TOKEN(TOKEN_LEFT_BRACKET); // check if token type is left bracket
        CHECK_ERROR(); // check for error of CHECK_TOKEN()
        
        /*Going to use next rule DEF ID ( <params> */
        err = params(parser);
        CHECK_ERROR();

        /* If everything went well we are in this state -> STATE: DEF ID ( <params> ): */
        GET_NEXT_TOKEN(); 
        CHECK_TOKEN(TOKEN_COLON); // expected ':'
        CHECK_ERROR();

        /* Expected state -> STATE: DEF ID ( <params> ): EOL */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_EOL); // expected end of line (EOL)
        CHECK_ERROR();

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_INDENT); // python indentation expected
        CHECK_ERROR();

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT <statement_inside> <end> */
        GET_NEXT_TOKEN();
        /* Rule <end> is implemented simply as an if condition which makes it easier */
        /* In this state two tokens are valid -> EOF/EOL others cause syntax_error */
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT <statement_inside> <end> DEDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_DEDENT); // python dedentation expected
        CHECK_ERROR();
    }

    /* Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement> */
    if(parser->curr_token.type == KEYWORD_IF)
    {
        is_if = true;
        // expression_parser function call

        /* STATE: IF <expression_start>: */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_COLON); // expected ':'
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_INDENT); // python dedentation expected
        CHECK_ERROR();

        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_DEDENT); 
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(KEYWORD_ELSE); // else statement always have tp be after if 
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_COLON); // expected ':'
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_INDENT); 
        CHECK_ERROR();

         /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> DEDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_DEDENT); 
        CHECK_ERROR();
    }

    /* Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> <end> DEDENT <statement> */
    if(parser->curr_token.type == KEYWORD_WHILE)
    {
        is_while = true;
        // expression_parser function call
        
        /* STATE: WHILE <expression_start>: */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_COLON); // expected ':'
        CHECK_ERROR();

        /* STATE: WHILE <expression_start>: EOL */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
        CHECK_ERROR();

        /* STATE: WHILE <expression_start>: EOL INDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_INDENT); // python dedentation expected
        CHECK_ERROR();

        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;

         /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end>  DEDENT */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_DEDENT); 
        CHECK_ERROR();
    }

    /* Rule 6. <statement> -> ID = <expression_start> <end> <statement> */
    if(parser->curr_token.type == TOKEN_IDENTIFIER)
    {
        GET_KEY(); // Identifier is a special key in global or local symtable

        parser->symbol_data = symtab_lookup(parser->global_table, key.str, &err);
        CHECK_ERROR();
        if(parser->symbol_data != NULL)
        {
            return SYNTAX_ERROR; 
        }
        
        parser->symbol_data = symtab_add(parser->global_table, key.str, &err); // add ID of variable into global table
        CHECK_ERROR(); // check for internal error of used function
        parser->symbol_data->symbol_type = SYMBOL_VAR; // specifying for variable
        parser->symbol_data->symbol_type = SYMBOL_DEFINED; 
        //parser->symbol_data->symbol_validity = SYMBOL_GLOBAL;

        /* STATE: ID = */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_ASSIGN); 
        CHECK_ERROR();

        // expression_parser function call
        // after expression call we can define our variable with more details (INT/FLOAT... value...)

        /* STATE: ID = <expression_start> <end> */
        GET_NEXT_TOKEN();
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 7. <statement> -> PASS <end> <statement> */
    if(parser->curr_token.type == KEYWORD_PASS)
    {
        /* STATE: PASS <end> */
        GET_NEXT_TOKEN();
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 8. <statement> -> PRINT ( <arg> ) <end> <statement> */
    if(parser->curr_token.type == KEYWORD_PRINT)
    {
        /* STATE: PRINT ( */
        GET_NEXT_TOKEN();
        CHECK_TOKEN(TOKEN_LEFT_BRACKET); 
        CHECK_ERROR();

        /* STATE: PRINT ( <arg> */
        err = arg(parser);
        CHECK_ERROR();
        
        /* STATE: PRINT ( <arg> ) <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }
    
    
    err = statement(parser);
    CHECK_ERROR();
    return NO_ERROR;
}

/** Rule 17. <params> -> ID <next_params> 
 *  Rule 18. <params> -> eps
*/
int params(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();
    
    /* Rule 18. <params> -> eps */
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR; /* STATE: DEF ID ( ) (e.g. <params> -> eps)*/ 
    
    /* Rule 17. <params> -> ID <next_params> */
    CHECK_TOKEN(TOKEN_IDENTIFIER);
    CHECK_ERROR();

    /* STATE: DEF ID ( <params> */
    GET_KEY(); // get the ID of variable (it's a key in symtable)

    parser->symbol_data = symtab_add(parser->local_table, key.str, &err); // insert parameters here into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR();
    parser->symbol_data->params = &key; //parameter added
    parser->symbol_data->symbol_type = SYMBOL_PARAM;
    //parser->symbol_data->symbol_validity = SYMBOL_LOCAL;
    
    err = next_params(parser); // next rule
    CHECK_ERROR(); // always check the ret value

    return NO_ERROR;
}

/** Rule 19. <next_params> -> , ID <next_params> 
 *  Rule 20. <next_params> -> eps
*/
int next_params(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();

    /* Rule 20. <next_params> -> eps */
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR; /* STATE: DEF ID ( <params> ) */

    /* Rule 19. <next_params> -> , ID <next_params> */
    CHECK_TOKEN(TOKEN_COMMA); // expected ',' after parameter or syntax error
    CHECK_ERROR();

    /* STATE: DEF ID ( <params> , */
    GET_NEXT_TOKEN();
    CHECK_TOKEN(TOKEN_IDENTIFIER);
    CHECK_ERROR();
    
    /* STATE: DEF ID (<params> , <params> ... */
    GET_KEY(); // get the ID of variable (it's a key in symtable)

    parser->symbol_data = symtab_lookup(parser->local_table, key.str, &err);
    CHECK_ERROR();
    if((parser->symbol_data != NULL) && (parser->symbol_data->symbol_type == SYMBOL_PARAM)) // parameter with same ID was found in our local table which indicates syntax error
    {
        return SYNTAX_ERROR; // this causes error -> def foo(a,a): ...
    }

    parser->symbol_data = symtab_add(parser->local_table, key.str, &err); // insert parameters into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR();
    parser->symbol_data->params = &key; //parameter added
    parser->symbol_data->symbol_type = SYMBOL_PARAM; // symbol type set as parameter
    //parser->symbol_data->symbol_validity = SYMBOL_LOCAL;

    err = next_params(parser); // recursively go to next rule
    CHECK_ERROR(); // always check the return value

    return NO_ERROR;
}

int statement_inside(Parser *parser)
{
    int err;
}

/** Rule 37. <arg> -> eps
 *  Rule 38. <arg> -> <value> <arg>
*/
int arg(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();

    /*  Rule 37. <arg> -> eps */
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR; /* STATE: PRINT (e.g. <arg> -> eps) */ 

    err = arg(parser);
    CHECK_ERROR();

    return NO_ERROR;
}