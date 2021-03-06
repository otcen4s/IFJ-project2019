/*
* Project           : IFJ19 Compiler
* File              : parser.c
* Author            : Matej Otčenáš
*/

#include "parser.h"

/************** MACROS **********/
#define CHECK_ERROR() if(err) return err

#define SKIP_EOL() while(parser->curr_token.type == TOKEN_EOL) GET_NEXT_TOKEN()

#define NOT_NULL(_a,_b) ((_a) ? (_a) : (_b))

#define COPY_CURRENT_TOKEN()                                                                                                        \
                            do{                                                                                                     \
                                parser->previous_token = parser->curr_token;                                                        \
                                if(parser->previous_token.type == TOKEN_STRING || parser->previous_token.type == TOKEN_IDENTIFIER)  \
                                {                                                                                                   \
                                    str_copy(&(parser->prev_key), parser->previous_token.attribute.string.str);                     \
                                    parser->previous_token.attribute.string= parser->prev_key;                                      \
                                }                                                                                                   \
                                CHECK_ERROR();                                                                                      \
                            }while(0)

#define STORE_NEXT_TOKEN()                                                                                                      \
                        do{                                                                                                     \
                            parser->previous_token = read_token(parser->scanner, &err);                                         \
                            if(parser->previous_token.type == TOKEN_STRING || parser->previous_token.type == TOKEN_IDENTIFIER)  \
                            {                                                                                                   \
                                str_copy(&(parser->prev_key), parser->previous_token.attribute.string.str);                     \
                                parser->previous_token.attribute.string= parser->prev_key;                                      \
                            }                                                                                                   \
                            CHECK_ERROR();                                                                                      \
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

#define GET_KEY() str_copy(&(parser->key),parser->curr_token.attribute.string.str)

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
    (symtab_init(&(parser->local_table)))||
    (str_init(&(parser->key))) || 
    (str_init(&(parser->prev_key))))
    return INTERNAL_ERROR;

    int err= NO_ERROR;
    parser->left_side=NULL;
    parser->curr_token.type = -1;
    parser->symbol_data_local = NULL;
    parser->symbol_data_global = NULL; 
    parser->current_function = NULL;
    parser->is_in_def = false;
    parser->if_expression = false;
    parser->while_expression = false;
    parser->is_in_print = false;
    parser->is_in_return = false;
    parser->no_assign_expression = false;
    parser->expr_parser_call = false;

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

bool is_global(tSymbol_item * var, Parser * parser)
{
    int err= NO_ERROR;
    if(parser->is_in_def)
    {
        if (symtab_lookup(parser->local_table, var->key, &err)) return false;
        if (symtab_lookup(parser->global_table, var->key, &err)) return true; 
    }
    if (symtab_lookup(parser->global_table, var->key, &err)) return true; 
    return false; 
}

int check_function_defined(Parser *parser)
{
    for(size_t i = 0; i < SYMTAB_SIZE; i++)
    {
        struct tSymbol_item *current = parser->global_table->item_array[i];
        struct tSymbol_item *next = NULL;
        
        while(current != NULL)
        {
            next = current->next_symbol;

            if((current->symbol_type == SYMBOL_FUNC) && (current->symbol_state == SYMBOL_USED)) return UNDEFINE_REDEFINE_ERROR;
    
            current = next;
        }
    }

    return NO_ERROR;
}

int curr_key_is_defined(Parser *parser, unsigned sym_type)
{
    int err;
    parser->symbol_data_global = parser->symbol_data_local = NULL;

    //look for symbol in relevant tables based on current scope
    if(parser->is_in_def)
    {
        parser->symbol_data_local = symtab_lookup(parser->local_table, parser->key.str, &err);
        CHECK_ERROR();

        if(parser->symbol_data_local == NULL)
        {
            parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
            CHECK_ERROR();
        }
    }
    else
    {
        parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
        CHECK_ERROR();
    }
    
    //cases when its undefined
    if((!parser->symbol_data_local) &&(!parser->symbol_data_global)) return UNDEFINE_REDEFINE_ERROR;    
    tSymbol_item * checked = NOT_NULL(parser->symbol_data_local, parser->symbol_data_global);
    if((checked->symbol_state == SYMBOL_USED) && (sym_type == SYMBOL_VAR)) return UNDEFINE_REDEFINE_ERROR;
    if(checked->symbol_type != sym_type) return UNDEFINE_REDEFINE_ERROR;

    //its defined
    return NO_ERROR;    
}

void dispose_parser(Parser* parser)
{
    free(parser->scanner);
    str_destroy(&(parser->key));
    str_destroy(&(parser->prev_key));
    symtab_free(parser->global_table);
    symtab_free(parser->local_table);
}

// this function  will be the interface of parser (e.i. will be called from outside) 
int start_compiler(char* src_file_name, char* out_file_name)
{
    int err;

    Parser p;
    Parser *parser = &p; 
    if(init_parser(parser))  return INTERNAL_ERROR;
    
    generator_begin();

    //initialisation of scanner 
    if(init_scanner(parser->scanner, src_file_name)) return INTERNAL_ERROR;

    err = statement(parser);

    //check error but also memory need to be freed here
    if(err)
    {
        destroy_scanner(parser->scanner);
        dispose_parser(parser);
        return err;
    }

    err = check_function_defined(parser);
    CHECK_ERROR();

    // set output file
    FILE *output_file;
    if(strcmp(out_file_name, "stdout") == 0)
    {
        output_file= stdout;
    }
    else
    {
        if(fopen(out_file_name, "w")== NULL) return INTERNAL_ERROR;
    }

    generate_code(output_file);
    destroy_scanner(parser->scanner);
    dispose_parser(parser);
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
 * Rule 8. <statement> -> <value> <end> <statement> 
*/
int statement(Parser *parser)
{
    symtab_clear(parser->local_table);
    parser->left_side = NULL;
    parser->is_in_def = parser->is_in_print = parser->no_assign_expression = parser->expr_parser_call = false;
    int err;

    GET_NEXT_TOKEN();

    /* Return used outside function */
    if(parser->curr_token.type == KEYWORD_RETURN)
    {
        return SYNTAX_ERROR;
    }

    /* Rule 1. <statement> -> EOF */
    else if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
  
    /* Rule 2. <statement> -> EOL <statement>*/
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

        parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
        CHECK_ERROR();

        if((parser->symbol_data_global != NULL)                     && 
        (parser->symbol_data_global->symbol_type == SYMBOL_FUNC)    && 
        (parser->symbol_data_global->symbol_state == SYMBOL_USED))
        {
            used_function = true;
            parser->symbol_data_global->symbol_state = SYMBOL_DEFINED;
            parser->symbol_data_global->params_count_defined = 0;
        }
        
        else
        {
            if((parser->symbol_data_global != NULL) && (parser->symbol_data_global->symbol_type == SYMBOL_FUNC))
            {
                return UNDEFINE_REDEFINE_ERROR;
            }
            
            parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err); // add ID of function into global table
            CHECK_ERROR(); // check for internal error of used function
            parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
            parser->symbol_data_global->symbol_state = SYMBOL_DEFINED;
            parser->symbol_data_global->params_count_defined = 0;
            parser->symbol_data_global->params_count_used = 0;
        }

        gen_func_def_start(parser->key.str); //generator call

        /* STATE: DEF ID ( */
        GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET); // check if token type is left bracket
       
        
        /*Going to use next rule DEF ID ( <params> */
        err = params(parser);
        CHECK_ERROR();

        if(used_function) // expands to if our function was firstly used and then defined
        {
            if(parser->symbol_data_global->params_count_defined != 
               parser->symbol_data_global->params_count_used)
            {
                return PARAM_COUNT_ERROR;
            }
        }

        /* If everything went well we are in this state -> STATE: DEF ID ( <params> ): */
        GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* Expected state -> STATE: DEF ID ( <params> ): EOL */
        GET_CHECK_TOKEN(TOKEN_EOL);  // expected end of line (EOL)

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT */
        SKIP_EOL(); // in case there is more than one eol before indent
        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();

        /* Expected state -> STATE: DEF ID ( <params> ): EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        gen_func_def_end();

        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> DEDENT */
        CHECK_TOKEN(TOKEN_DEDENT);
        CHECK_ERROR();
    }

    /* Rule 4. <statement> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_IF)
    {
        parser->if_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->if_expression = false;

        gen_if_start(); // generator call

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN();
        }

        /* STATE: IF <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        //GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* STATE: IF <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();
        

        err = statement_inside(parser);
        CHECK_ERROR();

        if(parser->curr_token.type == TOKEN_EOF) return SYNTAX_ERROR;
        else if(parser->curr_token.type == TOKEN_DEDENT) err = NO_ERROR;

        gen_if_end(); // generator call

        
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE */
        GET_CHECK_TOKEN(KEYWORD_ELSE); // else statement always have tp be after if 
        

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: */
        GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();

        gen_else_start(); // generator call

         /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        gen_else_end(); // generator call

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> */
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> DEDENT */
        CHECK_TOKEN(TOKEN_DEDENT);
        CHECK_ERROR();
    }

    /* Rule 5. <statement> -> WHILE <expression_start>: EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_WHILE)
    {
        gen_while_start();

        parser->while_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->while_expression = false;

        gen_while_eval();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN();
        }

        /* STATE: WHILE <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        
        /* STATE: WHILE <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)


        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();


        err = statement_inside(parser);
        CHECK_ERROR();

        gen_while_end();

        /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end> */
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
         /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end>  DEDENT */
        CHECK_TOKEN(TOKEN_DEDENT); 
        CHECK_ERROR();
    }

    /* Rule 6. <statement> -> ID = <expression_start> <end> <statement> */
    /* In this rule is also implemented part of the 9th rule because of ID collission */
    else if(parser->curr_token.type == TOKEN_IDENTIFIER)
    {
        GET_KEY(); // Identifier is a special key in global or local symtable
        
        parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
        CHECK_ERROR();

        COPY_CURRENT_TOKEN(); 

        GET_NEXT_TOKEN();

        if(parser->curr_token.type == TOKEN_ASSIGN) /* STATE: ID = */
        {   
            if(parser->symbol_data_global == NULL)
            {
                parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err); // add ID of variable into global table
                CHECK_ERROR(); // check for internal error of used function
                parser->symbol_data_global->symbol_type = SYMBOL_VAR; // specifying for variable
                parser->symbol_data_global->symbol_state = SYMBOL_USED;

                gen_defvar(parser->key.str, true); // CODE GENERATOR
            }
            else if((parser->symbol_data_global != NULL) && (parser->symbol_data_global->symbol_type == SYMBOL_FUNC))
            {
                return UNDEFINE_REDEFINE_ERROR;
            }

            parser->left_side = parser->symbol_data_global; // this is for expression parser the left side ID to store value
            
            err = expression_start(parser);
            CHECK_ERROR();

            parser->left_side->symbol_state = SYMBOL_DEFINED;

            /* STATE: ID = <expression_start> <end> */
            if(!(parser->expr_parser_call))
            {
                GET_NEXT_TOKEN();
            }
            if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
            else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
            else return SYNTAX_ERROR;
        }

        /* STATE: ID */
        /* Part of expression_start rule is implemented here for this uniq state */
        else 
        {
            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
                CHECK_ERROR();
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err);
                        CHECK_ERROR();
                        parser->symbol_data_global->symbol_state = SYMBOL_USED;
                        parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
                        parser->symbol_data_global->params_count_used = 0;
                    }
                }                
                else if(parser->symbol_data_global == NULL) //undeclared function call outside func def = error 3
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }
                
                //this name was alredy defined but not as a function (error code 3)
                if (parser->symbol_data_global->symbol_type != SYMBOL_FUNC) return UNDEFINE_REDEFINE_ERROR; 
                
                //save symbol as current function to use it later  
                parser->current_function = parser->symbol_data_global;

                gen_func_call_start(); //code genrator call

                parser->current_function->params_count_used = 0;
                err = arg(parser);
                CHECK_ERROR();

                /* Checking the count of arguments */
                if((parser->current_function->symbol_state == SYMBOL_DEFINED)&& 
                (!(parser->is_in_print)))
                {
                    if(parser->current_function->params_count_defined != parser->current_function->params_count_used)
                    {
                        return PARAM_COUNT_ERROR;
                    }
                }

                //generate function call end (also moves retval into left side variable if there is one ) 
                gen_func_call_end(parser->current_function->key, 
                parser->left_side ? parser->left_side->key : NULL , 
                parser->left_side ? is_global(parser->left_side, parser) : false);
            }

            else
            {
                if(parser->symbol_data_global == NULL)
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                parser->no_assign_expression = true;
                
                err = expression_start(parser);
                CHECK_ERROR();

                if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
                else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
                else return SYNTAX_ERROR;
            }
        }
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

    /* Rule 8. <statement> -> <value> <end> <statement> */
    else
    {
        COPY_CURRENT_TOKEN();

        parser->no_assign_expression = true;

        err = expression_start(parser);
        CHECK_ERROR();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN();
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

    parser->symbol_data_local = symtab_add(parser->local_table, parser->key.str, &err); // insert parameters here into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR();
    parser->symbol_data_local->symbol_type = SYMBOL_VAR; //params
    parser->symbol_data_local->symbol_state = SYMBOL_DEFINED;
    parser->symbol_data_global->params_count_defined++;

    gen_func_def_add_param(parser->key.str);   //generator call
    
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

    parser->symbol_data_local = symtab_lookup(parser->local_table, parser->key.str, &err);
    CHECK_ERROR(); //internal error
    if(parser->symbol_data_local != NULL) //alredy defined error 3
    {
        return UNDEFINE_REDEFINE_ERROR; // this causes error -> def foo(a,a): ...
    }

    parser->symbol_data_local = symtab_add(parser->local_table, parser->key.str, &err); // insert parameters into local symtable ... also overwriting parser->symbol_data
    CHECK_ERROR(); //internal error
    parser->symbol_data_local->symbol_type = SYMBOL_VAR; // params are just local variables
    parser->symbol_data_local->symbol_state = SYMBOL_DEFINED; // on call params are always defined
    parser->symbol_data_global->params_count_defined++;

    gen_func_def_add_param(parser->key.str); // generator call

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

    if(!(parser->no_assign_expression))
    {
        STORE_NEXT_TOKEN();

        if((parser->is_in_return) && ((parser->previous_token.type == TOKEN_EOL) || (parser->previous_token.type == TOKEN_EOF)))
        {
            gen_instruct("PUSHS nil@nil"); // generator call
            parser->expr_parser_call = true;
            parser->curr_token = parser->previous_token;
            return NO_ERROR;
        }
    }
    

    switch(parser->previous_token.type)
    {
        /* STATE: ID = ID */
        case TOKEN_IDENTIFIER:
            
            if(!(parser->no_assign_expression))
            {
                GET_NEXT_TOKEN();
            }

            /* STATE: ID = ID ( */
            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
                {
                    return SYNTAX_ERROR;
                }

                str_copy(&(parser->key),parser->previous_token.attribute.string.str);

                parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
                CHECK_ERROR();

                if((parser->symbol_data_global != NULL) && (parser->symbol_data_global->symbol_type != SYMBOL_FUNC))
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err);
                        CHECK_ERROR();
                        parser->symbol_data_global->symbol_state = SYMBOL_USED;
                        parser->symbol_data_global->symbol_type = SYMBOL_FUNC;
                        parser->symbol_data_global->params_count_used = 0;
                    }
                }
                
                else if((parser->symbol_data_global == NULL) || (parser->symbol_data_global->symbol_state != SYMBOL_DEFINED)) // TODO maybe chceck if parser->symbol_data_global != NULL
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                parser->current_function = parser->symbol_data_global;

                gen_func_call_start();
                
                parser->current_function->params_count_used = 0;
                err = arg(parser);
                CHECK_ERROR();

                /* Checking the count of arguments */
                if((parser->current_function != NULL)                         && 
                (parser->current_function->symbol_state == SYMBOL_DEFINED)    && 
                (parser->current_function->symbol_type == SYMBOL_FUNC)        &&
                (!(parser->is_in_print)))
                {
                    if(parser->current_function->params_count_defined != parser->current_function->params_count_used)
                    {
                        return PARAM_COUNT_ERROR;
                    }
                }

                //generate function call end 
                if(parser->left_side)
                {
                    gen_func_call_end(parser->current_function->key, parser->left_side->key, is_global(parser->left_side, parser));
                }
                    
                else
                {
                    gen_func_call_end(parser->current_function->key, NULL , false);
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
            parser->is_in_print = true;

            parser->current_function = symtab_lookup(parser->global_table, "print", &err);
            CHECK_ERROR();


            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);
            
            parser->current_function->params_count_used = 0;
            err = arg(parser);
            CHECK_ERROR();

            gen_print_end();
            
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
            gen_instruct("CREATEFRAME");

            if (parser->previous_token.type == KEYWORD_INPUTI) {
                gen_func_builtin_call_end("inputi");
            }
            if (parser->previous_token.type == KEYWORD_INPUTF) {
                gen_func_builtin_call_end("inputf");
            }
            if (parser->previous_token.type == KEYWORD_INPUTS) {
                gen_func_builtin_call_end("inputs");
            }

            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }

            break;

        case KEYWORD_LEN:

            parser->current_function = symtab_lookup(parser->global_table, "len", &err);
            CHECK_ERROR();

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);
            
            gen_instruct("CREATEFRAME");

            parser->current_function->params_count_used = 0;
            err = arg(parser);
            CHECK_ERROR();

            if(parser->current_function->params_count_used != parser->current_function->params_count_defined)
            {
                return PARAM_COUNT_ERROR;
            }

            gen_func_builtin_call_end("len");

            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }
            
            break;  

        case KEYWORD_SUBSTR:

            parser->current_function = symtab_lookup(parser->global_table, "substr", &err);
            CHECK_ERROR();

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            gen_instruct("CREATEFRAME");
            
            parser->current_function->params_count_used = 0;
            err = arg(parser);
            CHECK_ERROR();

            if(parser->current_function->params_count_used != parser->current_function->params_count_defined)
            {
                return PARAM_COUNT_ERROR;
            }
            
            gen_func_builtin_call_end("substr");

            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }

            break;

        case KEYWORD_ORD:

            parser->current_function = symtab_lookup(parser->global_table, "ord", &err);
            CHECK_ERROR();

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            gen_instruct("CREATEFRAME");

            parser->current_function->params_count_used = 0;
            err = arg(parser);
            CHECK_ERROR();

            if(parser->current_function->params_count_used != parser->current_function->params_count_defined)
            {
                return PARAM_COUNT_ERROR;
            }

            gen_func_builtin_call_end("ord");

            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }

            break;

        case KEYWORD_CHR:

            parser->current_function = symtab_lookup(parser->global_table, "chr", &err);
            CHECK_ERROR();

            if((parser->is_in_return) || (parser->if_expression) || (parser->while_expression))
            {
                return SYNTAX_ERROR;
            }

            GET_CHECK_TOKEN(TOKEN_LEFT_BRACKET);

            gen_instruct("CREATEFRAME");

            parser->current_function->params_count_used = 0;
            err = arg(parser);
            CHECK_ERROR();

            if(parser->current_function->params_count_used != parser->current_function->params_count_defined)
            {
                return PARAM_COUNT_ERROR;
            }

            gen_func_builtin_call_end("chr");

            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }

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

    GET_NEXT_TOKEN();

    /* Rule 37. <arg> -> eps ... expands to STATE: ID () */
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) return NO_ERROR;

    /* Rule 38. <arg> -> <value> <arg> */
    switch(parser->curr_token.type)
    {
    case TOKEN_IDENTIFIER:
        GET_KEY(); // save string of ID into key
        
        //this also sets parser->symbol_data_global and symbol_data_global (both null if not deffined else one of them not null)
        err = curr_key_is_defined(parser, SYMBOL_VAR);
        CHECK_ERROR();
         
        parser->current_function->params_count_used++;
        
        //find out if current id used as arg is global or local variable (it was checked that its defined allredy)
        bool curr_id_is_global = is_global(NOT_NULL(parser->symbol_data_global , parser->symbol_data_local),parser);                    

        if(parser->is_in_print)
        {
             gen_print(curr_id_is_global,parser->curr_token); //generator call
        }
        else
        {
            gen_func_call_add_param(parser->curr_token, curr_id_is_global); //generator call
        }
        

        break;

    case TOKEN_INTEGER:
    case TOKEN_DECIMAL:
    case TOKEN_STRING:
    case KEYWORD_NONE:
        
        parser->current_function->params_count_used++;
        
        if(parser->is_in_print)
        {
            gen_print(!parser->is_in_def, parser->curr_token); //generator call
        }
        else
        {
            gen_func_call_add_param(parser->curr_token, !parser->is_in_def); //generator call
        }
    
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }

    GET_NEXT_TOKEN();

    // check again if there is right bracket
    if(parser->curr_token.type == TOKEN_RIGHT_BRACKET) 
    {
        if(parser->is_in_print)
        {
            if(parser->left_side)
            {
                gen_move_retval(parser->left_side->key, is_global(parser->left_side, parser));
            }
            parser->is_in_print = false;
        }
        
        return NO_ERROR;
    }
    CHECK_TOKEN(TOKEN_COMMA);  /* STATE: ID ( <arg> , */
    CHECK_ERROR();

    if (parser->is_in_print) {
        gen_print_space();
    }
    
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
 * 15. <statement_inside> -> <value> <end> <statement> 
*/
int statement_inside(Parser *parser)
{
    parser->is_in_return = parser->is_in_print = parser->no_assign_expression = parser->expr_parser_call = false;
    parser->left_side = NULL;
    int err;

    GET_NEXT_TOKEN();

    if(parser->curr_token.type == TOKEN_DEDENT)
    {
        return NO_ERROR;
    }

    else if(parser->curr_token.type == TOKEN_EOL)
    {
        err = statement_inside(parser);
        CHECK_ERROR();
        return NO_ERROR;
    }

    /* Function definition in <statement_inside> is not valid */
    else if(parser->curr_token.type == KEYWORD_DEF)
    {
        return SYNTAX_ERROR;
    }

    /* Return is valid only in definition of function */
    /* Rule 13. <statement_inside> -> RETURN <expression_start> EOL <statement_inside> */
    else if(parser->curr_token.type == KEYWORD_RETURN)
    {
        if(!(parser->is_in_def))
        {
            return SYNTAX_ERROR;
        }

        else
        {   
            parser->is_in_return = true;

            /* STATE: <statement_inside> RETURN <expression_start> */
            err = expression_start(parser);
            CHECK_ERROR();

            gen_func_def_return(); //generator call

            if(!(parser->expr_parser_call))
            {
                GET_NEXT_TOKEN();
            }
            if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
            else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
            else return SYNTAX_ERROR;
        }
        
    }

    /* Rule 10. <statement_inside> -> IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE : EOL INDENT <statement_inside> <end> DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_IF)
    {
        
        parser->if_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->if_expression = false;
        
        gen_if_start(); // generator call

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN();
        }
        /* STATE: IF <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        
        /* STATE: IF <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)
    
        /* STATE: IF <expression_start>: EOL INDENT */
        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();
        
        err = statement_inside(parser);
        CHECK_ERROR();

        if(parser->curr_token.type == TOKEN_EOF) return SYNTAX_ERROR;
        else if(parser->curr_token.type == TOKEN_DEDENT) err = NO_ERROR;

        gen_if_end();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE */
        GET_CHECK_TOKEN(KEYWORD_ELSE); // else statement always have tp be after if 
        
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: */
        GET_CHECK_TOKEN(TOKEN_COLON); // expected ':'
        

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT */
        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR(); 
        
        gen_else_start();

         /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> */
        err = statement_inside(parser);
        CHECK_ERROR();

        gen_else_end();

        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> */
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        /* STATE: IF <expression_start>: EOL INDENT <statement_inside> EOL DEDENT ELSE: EOL INDENT <statement_inside> <end> DEDENT */
        CHECK_TOKEN(TOKEN_DEDENT);
        CHECK_ERROR();
    }

    /* Rule 11. <statement_inside> -> WHILE <expression_start>: EOL INDENT <statement_inside> EOL DEDENT <statement> */
    else if(parser->curr_token.type == KEYWORD_WHILE)
    {
        gen_while_start();

        parser->while_expression = true;
        err = expression_start(parser);
        CHECK_ERROR();
        parser->while_expression = false;

        gen_while_eval();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN(); 
        }

        /* STATE: WHILE <expression_start>: */
        CHECK_TOKEN(TOKEN_COLON);
        CHECK_ERROR();
        
        /* STATE: WHILE <expression_start>: EOL */
        GET_CHECK_TOKEN(TOKEN_EOL); // expected end of line(EOL)

        /* STATE: WHILE <expression_start>: EOL INDENT */
        SKIP_EOL();

        CHECK_TOKEN(TOKEN_INDENT);
        CHECK_ERROR();

        err = statement_inside(parser);
        CHECK_ERROR();

        gen_while_end();

        /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end> */
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;

         /* STATE: WHILE <expression_start>: EOL INDENT <statement_inside> <end>  DEDENT */
        CHECK_TOKEN(TOKEN_DEDENT);
        CHECK_ERROR();
    }

    /* Rule 12. <statement> -> ID = <expression_start> <end> <statement> */
    /* In this rule is also implemented part of the 16th rule because of ID collission */
    else if(parser->curr_token.type == TOKEN_IDENTIFIER)
    {
        GET_KEY(); // Identifier is a special key in global or local symtable

        if(parser->is_in_def)
        {
            parser->symbol_data_local = symtab_lookup(parser->local_table, parser->key.str, &err);
            CHECK_ERROR();

            if(parser->symbol_data_local == NULL)
            {
                parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
                CHECK_ERROR();
            }
        }
        else
        {
            parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
            CHECK_ERROR();
        }

        COPY_CURRENT_TOKEN();
        
        GET_NEXT_TOKEN();

        if(parser->curr_token.type == TOKEN_ASSIGN) /* STATE: ID = */
        {   
            if(parser->is_in_def)
            {
                if(parser->symbol_data_local == NULL)
                {
                    parser->symbol_data_local = symtab_add(parser->local_table, parser->key.str, &err); // add ID of variable into local table
                    CHECK_ERROR(); // check for internal error of used function
                    parser->symbol_data_local->symbol_type = SYMBOL_VAR; // specifying for variable
                    parser->symbol_data_local->symbol_type = SYMBOL_USED;
                    
                    gen_defvar(parser->key.str, false); 
                }
                else if((parser->symbol_data_local != NULL) && (parser->symbol_data_local->symbol_type == SYMBOL_FUNC))
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }
                parser->left_side = parser->symbol_data_local;
            }
            else
            {
                if(parser->symbol_data_global == NULL)
                {
                    parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err); // add ID of variable into global table
                    CHECK_ERROR(); // check for internal error of used function
                    parser->symbol_data_global->symbol_type = SYMBOL_VAR; // specifying for variable
                    parser->symbol_data_global->symbol_type = SYMBOL_USED; 

                    gen_defvar(parser->key.str, true); 
                }
                else if((parser->symbol_data_global != NULL) && (parser->symbol_data_global->symbol_type == SYMBOL_FUNC))
                {
                    return UNDEFINE_REDEFINE_ERROR;
                }

                parser->left_side = parser->symbol_data_global;
            }

            err = expression_start(parser);
            CHECK_ERROR();
            
            //expression was valid so left side is inited propperly now
            parser->left_side->symbol_state = SYMBOL_DEFINED;
            

            if(!(parser->expr_parser_call))
            {
                GET_NEXT_TOKEN(); // TODO check if we need this token to get
            }
            if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
            else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
            else return SYNTAX_ERROR;
        }

        /* STATE: ID */
        /* Part of expression_start rule is implemented here for this uniq state */
        else 
        {
            if(parser->curr_token.type == TOKEN_LEFT_BRACKET) // indicates it's an function call (not built_in function)
            {
                parser->symbol_data_global = symtab_lookup(parser->global_table, parser->key.str, &err);
                CHECK_ERROR();
                
                if(parser->is_in_def) //if we are in definition of function
                {
                    if((parser->symbol_data_global == NULL)) // if no ID with that key was found we can create one
                    {
                        parser->symbol_data_global = symtab_add(parser->global_table, parser->key.str, &err);
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

                parser->current_function = parser->symbol_data_global;

                gen_func_call_start();

                parser->current_function->params_count_used = 0;
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
                //generate function call end 
                //also moves retval into left side variable if there is one
                gen_func_call_end(parser->current_function->key, 
                                  parser->left_side?parser->left_side->key:NULL , false);
                
            }

            else
            {
                if(parser->is_in_def)
                {
                    if(parser->symbol_data_local == NULL)
                    {
                        if(parser->symbol_data_global == NULL)
                        {
                            return UNDEFINE_REDEFINE_ERROR;
                        }
                    }
                }
                else
                {
                    if(parser->symbol_data_global == NULL)
                    {
                        return UNDEFINE_REDEFINE_ERROR;
                    }
                }
                
                parser->no_assign_expression = true;

                parser->left_side = parser->symbol_data_global;
                
                err = expression_start(parser);
                CHECK_ERROR();

                if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
                else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
                else return SYNTAX_ERROR;
            }
        }
    }

    /* Rule 14. <statement_inside> -> PASS EOL <statement_inside> */
    else if(parser->curr_token.type == KEYWORD_PASS)
    {
        /* STATE: PASS <end> */
        GET_NEXT_TOKEN();

        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    else if(parser->curr_token.type == TOKEN_EOF)
    {
        return NO_ERROR;
    }

    /* Rule 16. <statement_inside> -> <value> <end> <statement> */
    else
    {
        COPY_CURRENT_TOKEN();

        parser->no_assign_expression = true;

        err = expression_start(parser);
        CHECK_ERROR();

        if(!(parser->expr_parser_call))
        {
            GET_NEXT_TOKEN();
        }
        if(parser->curr_token.type == TOKEN_EOF) return NO_ERROR;
        else if(parser->curr_token.type == TOKEN_EOL) err = NO_ERROR;
        else return SYNTAX_ERROR;
    }

    err = statement_inside(parser);
    CHECK_ERROR();

    return NO_ERROR;
}
