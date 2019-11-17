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
                                    tmp = symtab_add(parser->global_table, (id), &err);     \
                                    CHECK_ERROR();                                          \
                                    tmp->data_type = (type);                                \
                                    tmp->symbol_state = SYMBOL_DEFINED;                     \
                                    tmp->symbol_type = SYMBOL_FUNC;                         \
                                }while(0)

/********* PREDECLARATIONS ***********/
int statement (Parser *parser);
int statement_inside (Parser *parser);
int end (Parser *parser);
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
size_t nested_cnt = 0;


/****************** FUNCTION DEFINITIONS ************************/

int init_parser(Parser* parser)
{
    if(((parser->scanner = malloc(sizeof(Scanner))) == NULL) || (symtab_init(parser->global_table)) || (symtab_init(parser->local_table))) return INTERNAL_ERROR;

    int err;
    parser->curr_token.type = -1;
    parser->symbol_data = NULL;

    /* Predefine built-in functions into global table*/
    tSymbol_item* tmp;
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
 *  Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> EOL DEDENT <statement>
 *  Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> EOL DEDENT <statement>
 *  Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> EOL DEDENT <statement> 
 *  Rule 6. <statement> -> ID = <expression_start> EOL <statement>
 *  Rule 7. <statement> -> PASS EOL <statement>
 *  Rule 9. <statement> -> PRINT ( <arg> )
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

    /* Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> EOL DEDENT <statement> */
    if(parser->curr_token.type == KEYWORD_DEF)
    {
        
        GET_NEXT_TOKEN(); // ID
        /* STATE: DEF ID */
        CHECK_TOKEN(TOKEN_IDENTIFIER); // check if token type is ID
        CHECK_ERROR(); //always check error 
       
        GET_KEY(); // Identifier is a special key in global or local symtable
        
        parser->symbol_data = symtab_add(parser->global_table, key.str, &err); // add ID of function into global table
        CHECK_ERROR(); // check for internal error of used function
        parser->symbol_data->symbol_type = SYMBOL_FUNC; // specifying it's function
        parser->symbol_data->symbol_state = SYMBOL_DEFINED; // -||- defined

        GET_NEXT_TOKEN();
        /* STATE: DEF ID ( */
        CHECK_TOKEN(TOKEN_LEFT_BRACKET); // check if token type is left bracket
        CHECK_ERROR(); // check for error of CHECK_TOKEN()
        
        /*Going to use next rule DEF ID ( <params> */
        err = params(parser);
        CHECK_ERROR();
    }
}

/** Rule 17. <params> -> ID <next_params> 
 *  Rule 18. <params> -> eps
*/
int params(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();
    
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR; /* STATE: DEF ID ( ) (e.g. <params> -> eps)*/ 
    
    CHECK_TOKEN(TOKEN_IDENTIFIER);
    CHECK_ERROR();

    /* STATE: DEF ID ( <params> */
    GET_KEY(); // get the ID of variable (it's a key in symtable)
    // insert parameters here into local symtable ... also overwriting parser->symbol_data
    parser->symbol_data = symtab_add(parser->local_table, key.str, &err);
    func_str_insert(parser->symbol_data->function_params, &key); //parameter added 


    err = next_params(parser); // next rule
    CHECK_ERROR(); // always check the ret value
}

/** Rule 19. <next_params> -> , ID <next_params> 
 *  Rule 20. <next_params> -> eps
*/
int next_params(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();
    
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR; /* STATE: DEF ID ( <params> ) */

    CHECK_TOKEN(TOKEN_COMMA); // expected ',' after parameter or syntax error
    CHECK_ERROR();

    /* STATE: DEF ID ( <params> , */
    GET_NEXT_TOKEN();
    CHECK_TOKEN(TOKEN_IDENTIFIER);
    CHECK_ERROR();
    
    /* STATE: DEF ID (<params> , <params> ... */
    // save identifier into buffer

    err = next_params(parser); // recursively go to next rule
    CHECK_ERROR(); // always check the return value
}
