#include "scanner.h"

int init_scanner(Scanner *s, const char* file_name)
{ 
    if(strcmp(file_name, "stdin") == 0)
    {
        s->src_file=stdin; 
    }
    else 
    {
        if(!(s->src_file=fopen(file_name, "r")))
        {
            return INTERNAL_ERROR;
        }
    }
    s->atr_string= (tString*) malloc(sizeof(tString));
    if (s->src_file == NULL || s->atr_string == NULL) return INTERNAL_ERROR;
    s->is_line_start= 1;
    s->state= STATE_START;
    s->curr_char=0;

    //if(!(s->ungeted_stack = stack_create(TOKEN_TYPE)))

    //init string buffer
    if(str_init(s->atr_string)) return INTERNAL_ERROR; 
    
    //init stack for indent analysis
     s->stack = stack_create(100, INTEGER_TYPE);
     stack_push(s->stack, 0); 
    return NO_ERROR;
}

void destroy_scanner (Scanner *s)
{ 
    str_destroy(s->atr_string); 
    free(s->atr_string);
    stack_free(s->stack);
}

void check_keyword(tString* string, Token* token){
    if(str_cmp_keyword(string, "def")) token->type = KEYWORD_DEF;
    else if(str_cmp_keyword(string, "else")) token->type = KEYWORD_ELSE;
    else if(str_cmp_keyword(string, "if")) token->type = KEYWORD_IF;
    else if(str_cmp_keyword(string, "None")) token->type = KEYWORD_NONE;
    else if(str_cmp_keyword(string, "pass")) token->type = KEYWORD_PASS;
    else if(str_cmp_keyword(string, "return")) token->type = KEYWORD_RETURN;
    else if(str_cmp_keyword(string, "while")) token->type = KEYWORD_WHILE;
    else if(str_cmp_keyword(string, "and")) token->type = KEYWORD_AND;
    else if(str_cmp_keyword(string, "or")) token->type = KEYWORD_OR;
    else if(str_cmp_keyword(string, "is")) token->type = KEYWORD_IS;
    else if(str_cmp_keyword(string, "not")) token->type = KEYWORD_NOT;
    else if(str_cmp_keyword(string, "true")) token->type = KEYWORD_TRUE;
    else if(str_cmp_keyword(string, "false")) token->type = KEYWORD_FALSE;
    else if(str_cmp_keyword(string, "print")) token->type = KEYWORD_PRINT;
    else if(str_cmp_keyword(string, "inputs")) token->type = KEYWORD_INPUTS;
    else if(str_cmp_keyword(string, "inputi")) token->type = KEYWORD_INPUTI; 
    else if(str_cmp_keyword(string, "inputf")) token->type = KEYWORD_INPUTF;
    else if(str_cmp_keyword(string, "substr")) token->type = KEYWORD_SUBSTR;
    else if(str_cmp_keyword(string, "ord")) token->type = KEYWORD_ORD;   
    else if(str_cmp_keyword(string, "chr")) token->type = KEYWORD_CHR; 
    else
    {
        token->type = TOKEN_IDENTIFIER;
        token->attribute.string = *string; //save identifier name as a token atribute 
    } 
}

Token create_integer_token(tString string, int *error) {
    *error = NO_ERROR;
    
    Token token;
    token.type = 0; 
    char *endptr;
    long  value = strtol(string.str, &endptr, 10);

    if (*endptr != '\0') {
        *error = INTERNAL_ERROR;
        return token;
    }

    token.type = TOKEN_INTEGER;
    token.attribute.integer = value;
    
    return token;
}

Token create_decimal_token(tString string) 
{
    
    Token token;
    double value = atof(string.str);
    token.type = TOKEN_DECIMAL;
    token.attribute.decimal = value;
    
    return token;
}

Token create_string_token(tString string) {    
    Token token;
    token.type = TOKEN_STRING;
    token.attribute.string=string;
    
    return token;
}
/*
int unget_token(Scanner *scanner, Token token)
{
    int idx = ++scanner->ungeted_count;
    scanner->ungeted_tokens[idx] = token;
}
*/

Token read_token(Scanner *scanner, int *err)
{
    scanner->state = STATE_START;
    
    Token  empty_token;
    empty_token.type=0;
    *err = NO_ERROR;
    
    //used to store hexadecimal sequence for conversion
    char hexa_number_str[3] = {0};

    //clear atribute string
    if(str_copy(scanner->atr_string, ""))
    {
        *err= INTERNAL_ERROR;
        return empty_token;
    }

    while(true)
    {
       //read character form file
       scanner->curr_char=getc(scanner->src_file);
       Token token;
       token.type=0; //initialisation

        switch (scanner->state)
        {
        
            case STATE_START:
                if (scanner->is_line_start) 
                {
                    scanner->state = STATE_INDENTATION_CHECK;   // indentation check
                    scanner->indent_cnt = 0; //init indent counter to zero
                    ungetc(scanner->curr_char, scanner->src_file); //nothing should be readed here
                }
                else if((scanner->curr_char == '\n') && 
                       (scanner->is_line_start)) scanner->state = STATE_START; //ignore empty lines   
                else if((scanner->curr_char == ' ') && 
                        (!scanner->is_line_start)) scanner->state = STATE_START; //ignore space if its not indent
                else if(scanner->curr_char == '+') scanner->state = STATE_PLUS;
                else if (scanner->curr_char == '*') scanner->state = STATE_MULTIPLICATION;
                else if (scanner->curr_char == '-') scanner->state = STATE_MINUS;
                else if (scanner->curr_char == ':') scanner->state = STATE_COLON;
                else if (scanner->curr_char == '[') scanner->state = STATE_LEFT_SQUARE_BRACKET;
                else if (scanner->curr_char == ']') scanner->state = STATE_RIGHT_SQUARE_BRACKET;
                else if (scanner->curr_char == '%') scanner->state = STATE_PERCENT;
                else if (scanner->curr_char == '/') scanner->state = STATE_DIVISON;    
                else if (scanner->curr_char == '>') scanner->state = STATE_GREATER_THAN;
                else if (scanner->curr_char == '<') scanner->state = STATE_LESSER_THAN;
                else if (scanner->curr_char == '(') scanner->state = STATE_LEFT_BRACKET;            
                else if (scanner->curr_char == ')') scanner->state = STATE_RIGHT_BRACKET;
                else if (scanner->curr_char == '=') scanner->state = STATE_ASSIGN;
                else if (scanner->curr_char == EOF) scanner->state = STATE_EOF;
                else if (scanner->curr_char == '\n' ) scanner->state = STATE_EOL;
                else if (scanner->curr_char == ',') scanner->state = STATE_COMMA;
                else if (scanner->curr_char == '#') scanner->state = STATE_COMMENT_SINGLE_LINE;
                else if (scanner->curr_char == '\'') scanner->state = STATE_STRING_START;
                else if  (scanner->curr_char == '"') scanner->state = STATE_DOCSTRING_ENTER_1;
                else if ((isalpha(scanner->curr_char))  || (scanner->curr_char == '_'))
                {
                    scanner->state = STATE_ID; //id or keyword, will be checked later
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else if ((isdigit(scanner->curr_char)))
                {
                    scanner->state = STATE_NUMBER;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }               
                else scanner->state = STATE_ERROR;
                
                break;           

            case STATE_INDENTATION_CHECK:                
                if (scanner->curr_char == ' ') scanner->indent_cnt++; //count number of spaces
                else
                {
                    scanner->state = STATE_INDENT_ENDED;
                    ungetc(scanner->curr_char, scanner->src_file);
                }                
                break;
            
            case STATE_INDENT_ENDED:
                ungetc(scanner->curr_char, scanner->src_file);

                //indentation matches previous            
                if(stack_top(scanner->stack, err).integer == scanner->indent_cnt)
                {
                    scanner->state= STATE_START;
                    scanner->is_line_start  =0;
                }
                //indentation increased, generate INDENT token
                else if(stack_top(scanner->stack, err).integer < scanner->indent_cnt)
                {
                    token.type = TOKEN_INDENT;
                    scanner->is_line_start  = 0;
                    stack_push(scanner->stack, scanner->indent_cnt);
                    return token; 
                }
                //indentation decreased, generate dedent 
                else if(stack_top(scanner->stack, err).integer > scanner->indent_cnt)
                {
                    token.type = TOKEN_DEDENT;
                    stack_pop(scanner->stack, err);
                    
                    //if is equal after pop generate dedent and set line start to 0 because indentation checking is done
                    if(stack_top(scanner->stack, err).integer == scanner->indent_cnt)
                    {
                        scanner->is_line_start  = 0;
                        return token; 
                    }
                    //more than one dedent will be generated
                    else if (stack_top(scanner->stack, err).integer > scanner->indent_cnt)
                    { 
                        //return readed spaces to the file to check other indents generation
                        while(scanner->indent_cnt)
                        { 
                            scanner->indent_cnt--; 
                            ungetc(' ', scanner->src_file);
                        }
                        return token;                       
                    }
                     //indentation does not match any on the stack
                    else
                    {
                        *err = LEXICAL_ERROR;
                        return empty_token;
                    }
                    
                }
                break; 


            case STATE_ERROR:
                *err = LEXICAL_ERROR;
                return token; 
                break;
            
            case STATE_PLUS:

                token.type=TOKEN_PLUS;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                break;
            
            case STATE_MINUS:
                token.type= TOKEN_MINUS;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_MULTIPLICATION:
                token.type= TOKEN_MULTIPLICATION;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_DIVISON:
                token.type= TOKEN_DIVISION;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;

            case STATE_GREATER_THAN:
                if(scanner->curr_char == '=')
                {
                    scanner->state = STATE_GREATER_EQUAL;
                }
                else
                {
                token.type= TOKEN_GREATER_THAN;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                }
                break;
            
            case STATE_LESSER_THAN:
                if(scanner->curr_char == '=')
                {
                    scanner->state = STATE_LESSER_EQUAL;
                }
                else
                {
                token.type= TOKEN_LESSER_THAN;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                }
                break;
            
            case STATE_GREATER_EQUAL:
                token.type= TOKEN_GREATER_EQUAL;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_LESSER_EQUAL:
                token.type= TOKEN_LESSER_EQUAL;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_COLON:
                token.type=TOKEN_COLON;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                break;
            
            case STATE_PERCENT:
                token.type=TOKEN_PERCENT;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                break;

            case STATE_EOL:
                token.type= TOKEN_EOL;
                ungetc(scanner->curr_char, scanner->src_file);
                scanner->is_line_start = 1;                
                return token;
                break;

            case STATE_EOF:
                token.type= TOKEN_EOF;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;

            case STATE_LEFT_BRACKET:
                token.type= TOKEN_LEFT_BRACKET;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_RIGHT_BRACKET:
                token.type= TOKEN_RIGHT_BRACKET;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;
            
            case STATE_RIGHT_SQUARE_BRACKET:
                token.type=TOKEN_RIGHT_SQUARE_BRACKET;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                break;
            
            case STATE_LEFT_SQUARE_BRACKET:
                token.type=TOKEN_LEFT_SQUARE_BRACKET;
                ungetc(scanner->curr_char, scanner->src_file);
                return token;
                break;

            case STATE_COMMA:
                token.type= TOKEN_COMMA;
                ungetc(scanner->curr_char, scanner->src_file);
                return token; 
                break;

            case STATE_COMMENT_SINGLE_LINE:
                if(scanner->curr_char == '\n' || scanner->curr_char  == EOF)
                {
                    ungetc(scanner->curr_char, scanner->src_file);
                    scanner->state = STATE_START;
                }
                break;

            case STATE_ASSIGN:
                if(scanner->curr_char == '=')
                {
                    scanner->state = STATE_EQUAL;
                } 
                else
                {
                    ungetc(scanner->curr_char, scanner->src_file);
                    token.type = TOKEN_ASSIGN;
                    return token; 
                }
                break;
            
            case STATE_EQUAL:
                token.type = TOKEN_EQUAL;
                return token;
                break;
            
                    
            case STATE_STRING_START:
                if(scanner->curr_char == '\'')
                {
                    scanner->state = STATE_STRING_END;
                }
                else if (scanner->curr_char == '\\')                {
                    scanner->state = STATE_STRING_ESCAPE_SEQ;
                }
                else if (scanner->curr_char < 32) //invalid character
                {
                    scanner->state = STATE_ERROR;
                    *err = INTERNAL_ERROR;
                }
         
                else
                {                    
                    if(str_insert_char(scanner->atr_string,scanner->curr_char))
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                break;


            case STATE_STRING_ESCAPE_SEQ:
                if(scanner->curr_char == 'n')
                {
                    if(str_insert_char(scanner->atr_string,'\n'))
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                    scanner->state= STATE_STRING_START;
                }
                else if (scanner->curr_char == 't')
                {
                    if(str_insert_char(scanner->atr_string,'\t'))
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                    scanner->state= STATE_STRING_START;
                }
                else if (scanner->curr_char == '\\')
                {
                    if(str_insert_char(scanner->atr_string,'\\'))
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                    scanner->state= STATE_STRING_START;
                }
                else if (scanner->curr_char == '\'')
                {
                    if(str_insert_char(scanner->atr_string,'\''))
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                    scanner->state= STATE_STRING_START;
                }
                // check for hexa decimal escape sequence
                else if (scanner->curr_char == 'x') scanner->state = STATE_STR_ESCAPE_HEXA_1;    
                else 
                {
                    str_insert_char(scanner->atr_string,'\\'); //TODO fail check
                    str_insert_char(scanner->atr_string,scanner->curr_char);
                    scanner->state = STATE_STRING_START; //continue reading the rest of the string
                }  
                break;
            
            case STATE_STR_ESCAPE_HEXA_1:
                if (
                  (tolower(scanner->curr_char) >= 'a' && tolower(scanner->curr_char) <= 'f') ||
                  (scanner->curr_char >= '0' && scanner->curr_char <= '9')
                   )
                {
                    hexa_number_str[0] = scanner->curr_char;
                    scanner->state = STATE_STR_ESCAPE_HEXA_2;
                }
                else
                { 
                    scanner->state= STATE_ERROR;
                }
                break; 

            

            case STATE_STR_ESCAPE_HEXA_2:
                if (
                        (tolower(scanner->curr_char) >= 'a' && tolower(scanner->curr_char) <= 'f') ||
                        (scanner->curr_char >= '0' && scanner->curr_char <= '9')
                    )
                {
                    hexa_number_str[1]=scanner->curr_char; 
                    char* endptr;
                    char result= (char) strtol(hexa_number_str, &endptr, 16);
                    
                    if(str_insert_char(scanner->atr_string,result)||
                       *endptr != '\0')                  
                    { 
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    };
                    scanner->state= STATE_STRING_START;
                }
                else
                // not valid hexa decimal number, will be writen into resulting string as if there was no escape sequence i.e. one by one
                { 
                   if(
                    str_insert_char(scanner->atr_string,'\\') || 
                    str_insert_char(scanner->atr_string,hexa_number_str[0]) ||
                    str_insert_char(scanner->atr_string,scanner->curr_char)
                    )
                    {
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                
                scanner->state = STATE_STRING_START; //continue reading the rest of the string          
            break;
            
            
            case STATE_STRING_END:
                ungetc(scanner->curr_char, scanner->src_file);
                token = create_string_token(*(scanner->atr_string));
                return token;
            break;

            
            case STATE_DOCSTRING_ENTER_1:
                if(scanner->curr_char == '"') scanner->state = STATE_DOCSTRING_ENTER_2; // so far "" was readed
                else
                 {
                     *err= LEXICAL_ERROR;
                     return empty_token; 
                 }
            break; 

            case STATE_DOCSTRING_ENTER_2:
                if(scanner->curr_char == '"') scanner->state = STATE_DOCSTRING_VALID; // docstring was correctly entered
                else
                 {
                     *err= LEXICAL_ERROR;
                     return empty_token; 
                 }
            break; 


            case STATE_DOCSTRING_VALID:
                if(scanner->curr_char == '"') scanner->state = STATE_DOCSTRING_EXIT_1;
            break;


            case STATE_DOCSTRING_EXIT_1:
                if(scanner->curr_char == '"') scanner->state = STATE_DOCSTRING_EXIT_2; // "" readed
                else scanner->state = STATE_DOCSTRING_VALID; //docstring wasnt exited yet
            break;
            

            case STATE_DOCSTRING_EXIT_2:
                if(scanner->curr_char == '"') scanner->state = STATE_START; //docstring was propperly exited
                else scanner->state = STATE_DOCSTRING_VALID; //docstring wasnt exited yet
            break;
    
            case STATE_ID:   
                if(isalnum(scanner->curr_char) || scanner->curr_char == '_')
                {          
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else //end of identifier or keyword
                { 
                    //set token type to identifier or keyword and set appropriate token argument 
                    check_keyword(scanner->atr_string, &token);
                    ungetc(scanner->curr_char, scanner->src_file);
                    return token; 
                }
            break;

            case STATE_NUMBER:
                if(isdigit(scanner->curr_char))
                {
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }

                else if(scanner->curr_char == '.')
                {
                    scanner->state = STATE_NUMBER_DECIMAL;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else if (tolower(scanner->curr_char) == 'e')
                { 
                    scanner->state = STATE_NUMBER_EXPONENT;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else
                {
                    ungetc(scanner->curr_char, scanner->src_file);

                    //what we have readed so far is a correct int number
                    token = create_integer_token(*scanner->atr_string, err);
                    if(*err)
                    { 
                        return empty_token; //error code is allredy set
                    }
                    else return token;
                }
                break;
            
            case STATE_NUMBER_DECIMAL:
                if(isdigit(scanner->curr_char))
                {
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else if (tolower(scanner->curr_char) == 'e')
                { 
                    scanner->state = STATE_NUMBER_EXPONENT;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    {
                        *err = INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else
                {
                    ungetc(scanner->curr_char, scanner->src_file);

                    //what we have readed so far is a correct int number
                    token = create_decimal_token(*scanner->atr_string);
                    return token; 
                }
                break;

            case STATE_NUMBER_EXPONENT:
                if((scanner->curr_char == '+') || (scanner->curr_char == '-'))
                {
                    scanner->state = STATE_EXPONENT_SIGN;
                    
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    { 
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }

                }
                else if(isdigit(scanner->curr_char))
                {
                    scanner->state = STATE_NUMBER_EXPONENT_VALID;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    { 
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else scanner->state = STATE_ERROR;
                break; 
            
            case STATE_EXPONENT_SIGN:
                if(isdigit(scanner->curr_char))
                {
                    scanner->state = STATE_NUMBER_EXPONENT_VALID;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    { 
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else scanner->state = STATE_ERROR; 
                break; 

            
            case STATE_NUMBER_EXPONENT_VALID:
                if(isdigit(scanner->curr_char))
                {
                    scanner->state = STATE_NUMBER_EXPONENT_VALID;
                    if(str_insert_char(scanner->atr_string, scanner->curr_char))
                    { 
                        *err= INTERNAL_ERROR;
                        return empty_token;
                    }
                }
                else
                {
                    ungetc(scanner->curr_char, scanner->src_file);  //TODO NEEDS A FIX
                    //what we have readed so far is a correct int number
                    token = create_decimal_token(*scanner->atr_string);
                    if(*err)
                    { 
                        return empty_token; //error code is allredy set
                    }
                    else return token;
                }
                break;       
     
     
        }    
    }
}
