#include "parser.h"

/************** MACROS **********/
#define CHECK_ERROR() if(err) return err

#define GET_NEXT_TOKEN() \
                        do{\
                            parser->curr_token = read_token(parser->scanner, &err); \
                            CHECK_ERROR(); \
                        }while(0)

#define CHECK_TOKEN(token) \
                         do{\
                            if(parser->curr_token.type != token) err = SYNTAX_ERROR; \                 
                            else err = NO_ERROR; \
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
    if (!parser) return INTERNAL_ERROR;
    
    parser->curr_token.type = -1;

    if( (parser->scanner = malloc(sizeof(Scanner))) == NULL ||
        (parser->global_table = malloc(sizeof(tSymbol))) == NULL ||
        (parser->local_table = malloc(sizeof(tSymbol))) == NULL
      )
    {
        return INTERNAL_ERROR;
    }
    return NO_ERROR;
}

void dispose_parser(Parser* parser)
{
    free(parser->scanner);
    free(parser->global_table);
    free(parser->local_table);
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
       
        tString key = parser->curr_token.attribute.string;
        
        generate_variable_key(&key); // generate uniq key 

        parser->symbol_data = symtab_add(parser->global_table, key.str, &err);
        CHECK_ERROR(); // check for internal error of used function
        parser->symbol_data->symbol_type = SYMBOL_FUNC;

       
        GET_NEXT_TOKEN(); // (
        /* STATE: DEF ID ( */
        CHECK_TOKEN(TOKEN_LEFT_BRACKET); // check if token type is left bracket
        CHECK_ERROR(); // check for error of CHECK_TOKEN()
        
        /* */
      //  params(parser);
    }
}

/* Rule 17. <params> -> ID <next_params> */
int params(Parser *parser)
{
    int err;
    GET_NEXT_TOKEN();
    /* STATE: DEF ID ( ) */ 
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR;
    
    CHECK_TOKEN(TOKEN_IDENTIFIER);
    CHECK_ERROR();

    /* or we are in this STATE: DEF ID ( <params> */
    
}
