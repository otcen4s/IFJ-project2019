#include "parser.h"

/************** MACROS **********/
#define GET_NEXT_TOKEN() \
                        do{\
                            parser->curr_token = read_token(parser->scanner, err); \
                            if(err) return err; \
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



/****************** FUNCTION DEFINITIONS ************************/

int init_parser(Parser* parser)
{
    parser->curr_token.type = -1;

    if( !(parser->scanner == malloc(sizeof(Scanner))) ||
        !(parser->global_table = malloc(sizeof(tSymbol))) ||
        !(parser->local_table = malloc(sizeof(tSymbol)))
      )
    {
        return INTERNAL_ERROR;
    }
}

int dispose_parser(Parser* parser)
{
    free(parser->scanner);
    free(parser->global_table);
    free(parser->local_table);
}

void generate_variable_key(tString *var)
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
    if(init_parser(parser)) return INTERNAL_ERROR;

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
}


/************************************ RULES ***********************************************/

int statement(Parser *parser)
{
    int *err;
    GET_NEXT_TOKEN();

    /* Rule 1. <statement> -> EOL */
    if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
    
    /* Rule 2. <statement> -> EOF <statement>*/
    if(parser->curr_token.type == TOKEN_EOL) statement(parser);

    /* Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> EOL DEDENT <statement> */
    if(parser->curr_token.type == KEYWORD_DEF)
    {
        
    }
}

