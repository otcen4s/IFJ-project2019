#include "parser.h"

/************** MACROS **********/
#define CHECK_ERROR() if(err) return err

#define STORE_NEXT_TOKEN()                                                              \
                        do{                                                             \
                            parser->previous_token = read_token(parser->scanner, &err); \
                            CHECK_ERROR();                                              \
                        }while(0)

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

#define GET_CHECK_TOKEN(token)                          \
                            do{                         \
                                GET_NEXT_TOKEN();       \
                                CHECK_TOKEN((token));   \
                                CHECK_ERROR();          \
                            }while(0)

/********* PREDECLARATIONS ***********/
int statement (Parser *parser);
int statement_inside (Parser *parser);
int params (Parser *parser);
int next_params (Parser *parser);
int expression_start (Parser *parser);
int arg (Parser *parser);

/****************** FUNCTION DEFINITIONS ************************/

int init_parser(Parser* parser)
{
    if(((parser->scanner = malloc(sizeof(Scanner))) == NULL) || 
    (symtab_init(&(parser->global_table))) || 
    (symtab_init(&(parser->local_table)))) 
    return INTERNAL_ERROR;

    int err;
    parser->curr_token.type = -1;
    parser->symbol_data_local = NULL;
    parser->symbol_data_global = NULL; 
    parser->left_side = NULL;
    parser->is_in_def = false;
    parser->is_in_if = false;
    parser->if_expression = false;
    parser->is_in_while = false;
    parser->while_expression = false;
    parser->is_in_print = false;
    parser->is_in_return = false;
    parser->no_assign_expression = false;
    parser->expr_parser_call = false;
    parser->nested_cnt = 0;

    /* Predefine built-in functions into global table*/
    tSymbol_item* tmp;                      
    ADD_BUILT_IN_FUNC("print",DATA_NONE);
    ADD_BUILT_IN_FUNC("len",DATA_INT);
    tmp->params_count_defined = 1;
    ADD_BUILT_IN_FUNC("substr",DATA_STRING);
    tmp->params_count_defined = 3;
    ADD_BUILT_IN_FUNC("ord",DATA_INT);
    tmp->params_count_defined = 2;
    ADD_BUILT_IN_FUNC("chr",DATA_STRING);
    tmp->params_count_defined = 1;
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

/*void generate_variable_key(tString* var)
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
}*/


// this function  will be the interface of parser (e.i. will be called from outside) 
int start_compiler(char* src_file_name)
{
    //create and initialise parser data structure 
    Parser p;
    Parser *parser = &p; 
    if(init_parser(parser))  return INTERNAL_ERROR;
    
    return NO_ERROR;    
}


/************************************ RULES ***********************************************/

/** 
 * Rule 1. <statement> -> EOF
 * Rule 2. <statement> -> EOL <statement>
 * Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> DEDENT <statement>
 * Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement>
 * Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> <end> DEDENT <statement> 
 * Rule 6. <statement> -> ID = <expression_start> <end> <statement>
 * Rule 7. <statement> -> PASS <end> <statement>
 * Rule 8. <statement> -> PRINT ( <arg> ) <end> <statement>
 * Rule 9. <statement> -> <value> <end> <statement> 
*/
int statement(Parser *parser)
{
    symtab_clear(parser->local_table);
    parser->is_in_def = parser->is_in_if = parser->is_in_while = parser->is_in_print = parser->no_assign_expression = parser->expr_parser_call = false;
    int err;

    GET_NEXT_TOKEN();

    /* Return used outside function */
    if(parser->curr_token.type == KEYWORD_RETURN)
    {
        return SYNTAX_ERROR;
    }

    /* Rule 1. <statement> -> EOL */
    else if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
  
    /* Rule 2. <statement> -> EOF <statement>*/
    else if(parser->curr_token.type == TOKEN_EOL) 
    {
        err = statement(parser);
        CHECK_ERROR();
    }

    /* Rule 3. <statement> -> DEF ID ( <params> ): EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_DEF)
    {
        parser->is_in_def = true; // we are in the definition of function
        bool used_function = false; // help variable for detection of used function before definition
        
        /* STATE: DEF ID */
        GET_CHECK_TOKEN(TOKEN_IDENTIFIER);
       
        GET_KEY(); // Identifier is a special key in global or local symtable

        parser->symbol_data_global = symtab_lookup(parser->global_table, key.str, &err);
        CHECK_ERROR();

        if((parser->symbol_data_global != NULL)                     && 
        (parser->symbol_data_global->symbol_type == SYMBOL_FUNC)    && 
        (parser->symbol_data_global->symbol_state == SYMBOL_USED))
        {
            used_function = true;
        }
        
        parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err); // add ID of function into global table
        CHECK_ERROR(); // check for internal error of used function
        parser->symbol_data_global->symbol_type = SYMBOL_FUNC; // specifying it's function
        parser->symbol_data_global->symbol_state = SYMBOL_DEFINED; // -||- defined
        parser->symbol_data_global->params_count_defined = 0;

        /* STATE: DEF ID ( */
        GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET); // check if token type is left bracket
       
        
        /*Going to use next rule DEF ID ( <params> */
        err = params(parser);
        CHECK_ERROR();

        if(used_function) // expands to if our function was firstly used and then defined
        {
            if(parser->symbol_data_global->params_count_defined != parser->symbol_data_global->params_count_used)
            {
                return PARAM_COUNT_ERROR;
            }
        }

        /* If everything went well we are in this state -> STATE: DEF ID ( <params> ): */
        GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* Expected state -> STATE: DEF ID ( <params> ): EOL */
        GET_CHECK_TOKEN(TOKEN_EOL);  // expected end of line (EOL)

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT */
        GET_CHECK_TOKEN(TOKEN_INDENT); // python indentation expected

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
        GET_CHECK_TOKEN(TOKEN_DEDENT); // python dedentation expected
    }

    /* Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_IF)
    {
        parser->is_in_if = true;

        //STORE_NEXT_TOKEN();
        //GET_NEXT_TOKEN();
        parser->if_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->if_expression = false;

        /* STATE: IF <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        //GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* STATE: IF <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
    
        /* STATE: IF <expression_start>: EOL INDENT */
        GET_CHECK_TOKEN(TOKEN_INDENT); // python dedentation expected
        
        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT */
        GET_CHECK_TOKEN(TOKEN_DEDENT); 

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE */
        GET_CHECK_TOKEN(KEYWORD_ELSE); // else statement always have tp be after if 
        
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: */
        GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT */
        GET_CHECK_TOKEN(TOKEN_INDENT); 

         /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> DEDENT */
        GET_CHECK_TOKEN(TOKEN_DEDENT); 
    }

    /* Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_WHILE)
    {
        parser->is_in_while = true;
        
        //STORE_NEXT_TOKEN();
        //GET_NEXT_TOKEN();
        parser->while_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->while_expression = false;


        /* STATE: WHILE <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        //GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* STATE: WHILE <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        /* STATE: WHILE <expression_start>: EOL INDENT */
        GET_CHECK_TOKEN(TOKEN_INDENT); // python dedentation expected

        err = statement_inside(parser);
        CHECK_ERROR();

        /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;

         /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end>  DEDENT */
        GET_CHECK_TOKEN(TOKEN_DEDENT); 
    }

    /* Rule 6. <statement> -> ID = <expression_start> <end> <statement> */
    /* In this rule is also implemented part of the 9th rule because of ID collission */
    else if(parser->curr_token.type == TOKEN_IDENTIFIER)
    {
        GET_KEY(); // Identifier is a special key in global or local symtable
        
        parser->symbol_data_global = symtab_lookup(parser->global_table, key.str, &err);
        CHECK_ERROR();

        parser->previous_token = parser->curr_token; // storing ID in case of no assign 

        GET_NEXT_TOKEN();

        if(parser->previous_token.type == TOKEN_ASSIGN) /* STATE: ID = */
        {   
            if(parser->symbol_data_global == NULL)
            {
                parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err); // add ID of variable into global table
                CHECK_ERROR(); // check for internal error of used function
                parser->symbol_data_global->symbol_type = SYMBOL_VAR; // specifying for variable
                parser->symbol_data_global->symbol_state = SYMBOL_DEFINED; 
            }
            parser->left_side = parser->symbol_data_global; // this is for expression parser the left side ID to store value
            
            err = expression_start(parser);
            CHECK_ERROR();
        }

        /* STATE: ID */
        /* Part of expression_start rule is implemented here for this uniq state */
        else 
        {
            // previous_token = ID
            // current_token = anything else

            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                parser->symbol_data_global = symtab_lookup(parser->global_table, key.str, &err);
                CHECK_ERROR();
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err);
                        CHECK_ERROR();
                        parser->symbol_data_global->symbol_state = SYMBOL_USED;
                        parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
                        parser->symbol_data_global->params_count_used = 0;
                    }
                }
                
                else if((parser->symbol_data_global->symbol_state != SYMBOL_DEFINED)) // TODO maybe chceck if parser->symbol_data_global != NULL
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                err = arg(parser);
                CHECK_ERROR();

                /* Checking the count of arguments */
                if((parser->symbol_data_global != NULL)                         && 
                (parser->symbol_data_global->symbol_state == SYMBOL_DEFINED)    && 
                (parser->symbol_data_global->symbol_type == SYMBOL_FUNC)        &&
                (!(parser->is_in_print)))
                {
                    if(parser->symbol_data_global->params_count_defined != parser->symbol_data_global->params_count_used)
                    {
                        return PARAM_COUNT_ERROR;
                    }
                }
            }

            else
            {   
                parser->expr_parser_call = true;
                err = expression(parser); // calling function from expr_parser 
                CHECK_ERROR();
            }
        }

        /* STATE: ID = <expression_start> <end> */
        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN(); // TODO check if we need this token to get
        }
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 7. <statement> -> PASS <end> <statement> */
    else if(parser->curr_token.type == KEYWORD_PASS)
    {
        /* STATE: PASS <end> */
        GET_NEXT_TOKEN();
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 8. <statement> -> PRINT ( <arg> ) <end> <statement> */
    else if(parser->curr_token.type == KEYWORD_PRINT)
    {
        parser->is_in_print = true;
        /* STATE: PRINT ( */
        GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET); 

        /* STATE: PRINT ( <arg> */
        err = arg(parser);
        CHECK_ERROR();
        
        /* STATE: PRINT ( <arg> ) <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 9. <statement> -> <value> <end> <statement> */
    else
    {
        parser->no_assign_expression = true;

        err = expression_start(parser);
        CHECK_ERROR();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN(); // TODO check if we need this token to get
        }
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    
    err = statement(parser);
    CHECK_ERROR();
    return NO_ERROR;
}

/** 
 * Rule 17. <params> -> ID <next_params> 
 * Rule 18. <params> -> eps
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

    /* STATE: DEF ID ( ID */
    GET_KEY(); // get the ID of variable (it's a key in symtable)

    parser->symbol_data_local = symtab_add(parser->local_table, key.str, &err); // insert parameters here into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR();
    //parser->symbol_data->params = &key; //parameter added
    parser->symbol_data_local->symbol_type = SYMBOL_PARAM;
    parser->symbol_data_local->symbol_state = SYMBOL_DEFINED;
    parser->symbol_data_global->params_count_defined++;
    
    err = next_params(parser); // next rule
    CHECK_ERROR(); // always check the ret value

    return NO_ERROR;
}

/** 
 * Rule 19. <next_params> -> , ID <next_params> 
 * Rule 20. <next_params> -> eps
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
    GET_CHECK_TOKEN(TOKEN_IDENTIFIER);
    
    /* STATE: DEF ID (<params> , <params> ... */
    GET_KEY(); // get the ID of variable (it's a key in symtable)

    parser->symbol_data_local = symtab_lookup(parser->local_table, key.str, &err);
    CHECK_ERROR(); //internal error
    if((parser->symbol_data_local != NULL) && (parser->symbol_data_local->symbol_type == SYMBOL_PARAM)) // parameter with same ID was found in our local table which indicates syntax error
    {
        return SYNTAX_ERROR; // this causes error -> def foo(a,a): ...
    }

    parser->symbol_data_local = symtab_add(parser->local_table, key.str, &err); // insert parameters into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR(); //internal error
    //parser->symbol_data->params = &key; //parameter added
    parser->symbol_data_local->symbol_type = SYMBOL_PARAM; // symbol type set as parameter
    parser->symbol_data_global->params_count_defined++;

    err = next_params(parser); // recursively go to next rule
    CHECK_ERROR(); // always check the return value

    return NO_ERROR;
}


/**
 * Rule 21. <expression_start> -> <value> <expression_next>
 * Rule 22. <expression_next> -> eps (e.g. 'EOL')
 * Rule 23. <expression_next> -> OPERATOR <expression_start> 
 * Rule 24. <value> -> ID
 * Rule 25. <value> -> INT
 * Rule 26. <value> -> DOUBLE
 * Rule 27. <value> -> STRING
 * Rule 28. <value> -> NONE
 * Rule 29. <value> -> ID ( <arg> )
 * Rule 30. <value> -> INPUTS()
 * Rule 31. <value> -> INPTUF()
 * Rule 32. <value> -> INPUTI()
 * Rule 33. <value> -> LEN ( <value> )
 * Rule 34. <value> -> SUBSTR ( <value>, <value>, <value> )
 * Rule 35. <value> -> ORD ( <value>, <value> )
 * Rule 36. <value> -> CHR ( <value> )
*/
int expression_start(Parser *parser)
{
    int err;
    tString key;

    /* STATE: <value> */
    if(parser->no_assign_expression)
    {
        parser->previous_token = parser->curr_token;
    }
    /* STATE: ID = <value> */
    else
    {
        STORE_NEXT_TOKEN(); // same as GET_NEXT_TOKEN() but this stores token for later use
    }

    switch(parser->previous_token.type)
    {
        /* STATE: ID = ID */
        case TOKEN_IDENTIFIER:
            key = parser->curr_token.attribute.string;  
            
            GET_NEXT_TOKEN();
            /* STATE: ID = ID ( */
            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
                {
                    return SYNTAX_ERROR;
                }

                parser->symbol_data_global = symtab_lookup(parser->global_table, key.str, &err);
                CHECK_ERROR();
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err);
                        CHECK_ERROR();
                        parser->symbol_data_global->symbol_state = SYMBOL_USED;
                        parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
                        parser->symbol_data_global->params_count_used = 0;
                    }
                }
                
                else if((parser->symbol_data_global->symbol_state != SYMBOL_DEFINED)) // TODO maybe chceck if parser->symbol_data_global != NULL
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                err = arg(parser);
                CHECK_ERROR();

                /* Checking the count of arguments */
                if((parser->symbol_data_global != NULL)                         && 
                (parser->symbol_data_global->symbol_state == SYMBOL_DEFINED)    && 
                (parser->symbol_data_global->symbol_type == SYMBOL_FUNC)        &&
                (!(parser->is_in_print)))
                {
                    if(parser->symbol_data_global->params_count_defined != parser->symbol_data_global->params_count_used)
                    {
                        return PARAM_COUNT_ERROR;
                    }
                }
            }

            else
            {
                parser->expr_parser_call = true;
                err = expression(parser); // calling function from expr_parser 
                CHECK_ERROR();
            }
            
            break;

        case KEYWORD_PRINT:
            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            err = arg(parser);
            CHECK_ERROR();
            
            break;

        case TOKEN_STRING:
        case KEYWORD_NONE:
        case TOKEN_DECIMAL: 
        case TOKEN_INTEGER:
            GET_NEXT_TOKEN();

            parser->expr_parser_call = true;
            err = expression(parser);
            CHECK_ERROR();
            
            break;

        case KEYWORD_INPUTI:
        case KEYWORD_INPUTF:
        case KEYWORD_INPUTS:

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            GET_CHECK_TOKEN(TOKEN_RIGHT_BRACKET);


            //call code_generator

            break;

        case KEYWORD_LEN:

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);
            
            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_STRING)
            {
                key = parser->curr_token.attribute.string;
                // call generator
            }
            else
            {
                // error call
            }

            GET_CHECK_TOKEN(TOKEN_RIGHT_BRACKET);

            //GET_CHECK_TOKEN(TOKEN_EOL);
            
            break;  

        case KEYWORD_SUBSTR:

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);
            
            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_STRING)
            {
                key = parser->curr_token.attribute.string;
                // call generator
            }
            else
            {
                // error call
            }
            

            GET_CHECK_TOKEN(TOKEN_COMMA);

            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_INTEGER)
            {
                // call generator
            }
            else
            {
                //error call
            }
            

            GET_CHECK_TOKEN(TOKEN_COMMA);

            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_INTEGER)
            {
                // call generator
            }
            else
            {
                //error call
            }

            GET_CHECK_TOKEN(TOKEN_RIGHT_BRACKET);

            //GET_CHECK_TOKEN(TOKEN_EOL);
         
            break;
        case KEYWORD_ORD:

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_STRING)
            {
                key = parser->curr_token.attribute.string;
                // call generator
            }
            else
            {
                // error call
            }

            GET_CHECK_TOKEN(TOKEN_COMMA);

            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_INTEGER)
            {
                key = parser->curr_token.attribute.string;
                // call generator
            }
            else
            {
                // error call
            }

            GET_CHECK_TOKEN(TOKEN_RIGHT_BRACKET);

            //GET_CHECK_TOKEN(TOKEN_EOL);

            break;

        case KEYWORD_CHR:

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            GET_NEXT_TOKEN();
            if(parser->curr_token.type == TOKEN_IDENTIFIER)
            {
                key = parser->curr_token.attribute.string;
                //call generator
            }
            else if(parser->curr_token.type == TOKEN_INTEGER)
            {
                key = parser->curr_token.attribute.string;
                // call generator
            }
            else
            {
                // error call
            }

            GET_CHECK_TOKEN(TOKEN_RIGHT_BRACKET);

            //GET_CHECK_TOKEN(TOKEN_EOL);
            
            break;
        
        default:
            GET_NEXT_TOKEN();
            
            parser->expr_parser_call = true;
            err = expression(parser);
            CHECK_ERROR();
            
            break;
    }

    return NO_ERROR;
}

/** 
 * Rule 37. <arg> -> eps
 * Rule 38. <arg> -> <value> <arg>
*/
int arg(Parser *parser)
{
    int err;
    tString key;

    GET_NEXT_TOKEN();

    /* Rule 37. <arg> -> eps ... expands to STATE: ID () */
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR;

    /* Rule 38. <arg> -> <value> <arg> */
    switch(parser->curr_token.type)
    {
    case TOKEN_IDENTIFIER:
        key = parser->curr_token.attribute.string; // save string of ID into key
        
        parser->symbol_data_local = symtab_lookup(parser->global_table, key.str, &err);
        CHECK_ERROR();

        if((parser->symbol_data_local == NULL) || (parser->symbol_data_local->symbol_type == SYMBOL_FUNC) || (parser->symbol_data_local->symbol_state != SYMBOL_DEFINED))
        {
            return UNDEFINE_REDEFINE_ERROR; // trying to use undefined argument in function
        }

        if(!(parser->is_in_print)) // counting arguments number into global 
        {
            parser->symbol_data_global->params_count_used++;
        } 

        // calling generator 

        break;

    case TOKEN_INTEGER:
    case TOKEN_DECIMAL:
    case TOKEN_STRING:
    case KEYWORD_NONE:
        if(!(parser->is_in_print))
        {
            parser->symbol_data_global->params_count_used++;
        }
        // calling code generator
        break;

    default:
        return UNDEFINE_REDEFINE_ERROR; // TODO CHECK ERROR VALUE
        break;
    }

    GET_NEXT_TOKEN();

    // check again if there is right bracket
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR;
    CHECK_TOKEN(TOKEN_COMMA);  /* STATE: ID ( <arg> , */
    CHECK_ERROR();
    
    err = arg(parser);
    CHECK_ERROR();

    return NO_ERROR;
}



/**
 * 10. <statement_inside> -> IF <expression>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> EOL DEDENT <statement>
 * 11. <statement_inside> -> WHILE <expression_start>: EOL INDENT <statement_inside> EOL DEDENT <statement>
 * 12. <statement_inside> -> ID = <expression_start> EOL <statement_inside>
 * 13. <statement_inside> -> RETURN <expression_start> EOL <statement_inside>
 * 14. <statement_inside> -> PASS EOL <statement_inside>
 * 15. <statement_inside> -> PRINT ( <arg> ) EOL
 * 16. <statement_inside> -> <value> <end> <statement> 
*/
int statement_inside(Parser *parser)
{
    parser->is_in_return = false;
    int err;

    GET_NEXT_TOKEN();

    /* Function definition in <statement_inside> is not valid */
    if((parser->curr_token.type == KEYWORD_DEF) && (parser->is_in_def || parser->is_in_if || parser->is_in_while))
    {
        return SYNTAX_ERROR;
    }

    /* Return is valid only in definition of function */
    else if(parser->curr_token.type == KEYWORD_RETURN)
    {
        if(!(parser->is_in_def))
        {
            return SYNTAX_ERROR;
        }

        else
        {   
            parser->is_in_return = true;
            parser->left_side = parser->symbol_data_global;
            /* STATE: <statement_inside> RETURN <expression_start> */
            err = expression_start(parser);
            CHECK_ERROR();

            if(!(parser->expr_parser_call))
            {
                GET_NEXT_TOKEN(); // TODO check if we need this token to get
            }
            if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
            else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
            else return SYNTAX_ERROR;
        }
        
    }

    else if(parser->curr_token.type == KEYWORD_IF)
    {
        parser->nested_cnt++;
    }

    else if(parser->curr_token.type == KEYWORD_WHILE)
    {
        parser->nested_cnt++;
    }

    /* Rule 6. <statement> -> ID = <expression_start> <end> <statement> */
    /* In this rule is also implemented part of the 9th rule because of ID collission */
    else if(parser->curr_token.type == TOKEN_IDENTIFIER)
    {
        GET_KEY(); // Identifier is a special key in global or local symtable

        if(parser->is_in_def)
        {
            parser->symbol_data_local = symtab_lookup(parser->local_table, key.str, &err);
            CHECK_ERROR();

            if(parser->symbol_data_local == NULL)
            {
                parser->symbol_data_local = symtab_lookup(parser->global_table, key.str, &err);
                CHECK_ERROR();
            }
        }
        else
        {
            parser->symbol_data_global = symtab_lookup(parser->local_table, key.str, &err);
            CHECK_ERROR();
        }
        

        parser->previous_token = parser->curr_token; // storing ID in case of no assign 

        GET_NEXT_TOKEN();

        if(parser->previous_token.type == TOKEN_ASSIGN) /* STATE: ID = */
        {   
            if((parser->symbol_data_global == NULL) || (parser->symbol_data_local == NULL))
            {
                if(parser->is_in_def)
                {
                    parser->symbol_data_local = symtab_add(parser->local_table, key.str, &err); // add ID of variable into local table
                    CHECK_ERROR(); // check for internal error of used function
                    parser->symbol_data_local->symbol_type = SYMBOL_VAR; // specifying for variable
                    parser->symbol_data_local->symbol_type = SYMBOL_DEFINED; 
                }
                else
                {
                    parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err); // add ID of variable into global table
                    CHECK_ERROR(); // check for internal error of used function
                    parser->symbol_data_global->symbol_type = SYMBOL_VAR; // specifying for variable
                    parser->symbol_data_global->symbol_type = SYMBOL_DEFINED; 
                }
                
                
            }
            if(parser->is_in_def)
            {
                parser->left_side = parser->symbol_data_local;
            }
            else
            {    
                parser->left_side = parser->symbol_data_global; // this is for expression parser the left side ID to store value
            }

            err = expression_start(parser);
            CHECK_ERROR();
        }

        /* STATE: ID */
        /* Part of expression_start rule is implemented here for this uniq state */
        else 
        {
            // previous_token = ID
            // current_token = anything else

            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                parser->symbol_data_global = symtab_lookup(parser->global_table, key.str, &err);
                CHECK_ERROR();
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, key.str, &err);
                        CHECK_ERROR();
                        parser->symbol_data_global->symbol_state = SYMBOL_USED;
                        parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
                        parser->symbol_data_global->params_count_used = 0;
                    }
                }
                
                else if((parser->symbol_data_global->symbol_state != SYMBOL_DEFINED)) // TODO maybe chceck if parser->symbol_data_global != NULL
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                err = arg(parser);
                CHECK_ERROR();

                /* Checking the count of arguments */
                if((parser->symbol_data_global != NULL)                         && 
                (parser->symbol_data_global->symbol_state == SYMBOL_DEFINED)    && 
                (parser->symbol_data_global->symbol_type == SYMBOL_FUNC)        &&
                (!(parser->is_in_print)))
                {
                    if(parser->symbol_data_global->params_count_defined != parser->symbol_data_global->params_count_used)
                    {
                        return PARAM_COUNT_ERROR;
                    }
                }
            }

            else
            {
                parser->expr_parser_call = true;
                err = expression(parser); // calling function from expr_parser 
                CHECK_ERROR();
            }
        }

        /* STATE: ID = <expression_start> <end> */
        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN(); // TODO check if we need this token to get
        }
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 7. <statement> -> PASS <end> <statement> */
    else if(parser->curr_token.type == KEYWORD_PASS)
    {
        /* STATE: PASS <end> */
        GET_NEXT_TOKEN();
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 8. <statement> -> PRINT ( <arg> ) <end> <statement> */
    else if(parser->curr_token.type == KEYWORD_PRINT)
    {
        parser->is_in_print = true;
        /* STATE: PRINT ( */
        GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET); 

        /* STATE: PRINT ( <arg> */
        err = arg(parser);
        CHECK_ERROR();
        
        /* STATE: PRINT ( <arg> ) <end> */
        GET_NEXT_TOKEN(); // expected EOF or EOL and anything else is syntax_error
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    /* Rule 9. <statement> -> <value> <end> <statement> */
    else
    {
        parser->no_assign_expression = true;

        err = expression_start(parser);
        CHECK_ERROR();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN(); // TODO check if we need this token to get
        }
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    return NO_ERROR;
}
