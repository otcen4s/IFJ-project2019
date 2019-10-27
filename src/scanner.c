#include "scanner.h"

// declare global scanner
Scanner scan;
Scanner *scanner = &scan;


int init_scanner(Scanner *s, const char* file_name)
{ 
    if (s->src_file=fopen(file_name, "r")) return INTERNAL_ERROR;
    s->is_line_start= 1;
    s->state= STATE_START;
    s->curr_char=0; 
    s->atr_string= NULL;
    return NO_ERROR;
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
    else token->type = TOKEN_IDENTIFIER;
}

Token create_integer_token(tString string, int *error) {
    *error = NO_ERROR;
    
    Token token;
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

Token create_decimal_token(tString string, int *error) {
    *error = NO_ERROR;
    
    Token token;
    char *endptr;

    double value = strtod(string.str, &endptr);

    if (*endptr != '\0') {
        *error = INTERNAL_ERROR;
        return token;
    }

    token.type = TOKEN_DECIMAL;
    token.attribute.decimal = value;
    
    return token;
}

Token create_string_token(tString string, int *error) {
    *error = NO_ERROR;
    
    Token token;

    token.type = TOKEN_STRING;
    token.attribute.string=string;
    
    return token;
}

Token read_token(int *err)
{
    Token  empty_token;
    *err = NO_ERROR;
    
    //used to store hexadecimal sequence for conversion
    char hexa_number_str[3] = {0};

    //init atribute string
    if(str_init(scanner->atr_string))
    {
        return empty_token ; //returning empty token in case of error
        *err = INTERNAL_ERROR;
    } 


    while(true)
    {
       //read character form file
       scanner->curr_char=getc(scanner->src_file);
       Token token;

        switch (scanner->state)
        {
            case STATE_START: 
                if (scanner->is_line_start) scanner->state = STATE_INDENTATION_CHECK;   // indentation check
                else if((scanner->curr_char == ' ') && (!scanner->is_line_start)) scanner->state = STATE_START; //ignore space if its not indent    
                else if(scanner->curr_char == '+') scanner->state = STATE_PLUS;
                else if (scanner->curr_char == '*') scanner->state = STATE_MULTIPLICATION;
                else if (scanner->curr_char == '-') scanner->state = STATE_MINUS;  
                else if (scanner->curr_char == '/') scanner->state = STATE_DIVISON;    
                else if (scanner->curr_char == '>') scanner->state = STATE_GREATER_THAN;
                else if (scanner->curr_char == '<') scanner->state = STATE_RIGHT_BRACKET;
                else if (scanner->curr_char == '(') scanner->state = STATE_LEFT_BRACKET;            
                else if (scanner->curr_char == ')') scanner->state = STATE_RIGHT_BRACKET;
                else if (scanner->curr_char == '=') scanner->state = STATE_ASSIGN;
                else if (scanner->curr_char == EOF) scanner->state = STATE_EOF;
                else if (scanner->curr_char == '\n' ) scanner->state = STATE_EOL;
                else if (scanner->curr_char == ',') scanner->state = STATE_COMMA;
                else if (scanner->curr_char == '#') scanner->state = STATE_COMMENT_SINGLE_LINE;
                else if (isalpha(scanner->curr_char) || scanner->curr_char == '_' ) scanner->state = STATE_ID;
                else if (scanner->curr_char == '\'') scanner->state = STATE_STRING_START;
                else scanner->state = STATE_ERROR;      
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

            case STATE_EOL:
                token.type= TOKEN_EOL;
                ungetc(scanner->curr_char, scanner->src_file);
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
                    str_insert_char(scanner->atr_string,scanner->curr_char); //TODO fail check
                }
                break;

            case STATE_STRING_ESCAPE_SEQ:
                if(scanner->curr_char == 'n')
                {
                    str_insert_char(scanner->atr_string,'\n'); //TODO fail check
                }
                else if (scanner->curr_char == 't')
                {
                    str_insert_char(scanner->atr_string,'\t'); //TODO fail check
                }
                else if (scanner->curr_char == '\\')
                {
                    str_insert_char(scanner->atr_string,'\\'); //TODO fail check
                }
                else if (scanner->curr_char == '\'')
                {
                    str_insert_char(scanner->atr_string,'\''); //TODO fail check
                }
                // check for hexa decimal escape sequence
                else if (
                        (tolower(scanner->curr_char) >= 'a' && tolower(scanner->curr_char) <= 'f') ||
                        (scanner->curr_char >= '0' && scanner->curr_char <= '9')
                        )
                {
                    scanner->state = STATE_STR_ESCAPE_HEXA;
                    hexa_number_str[0] = scanner->curr_char;
                }
                //invalid escape sequence will be inserted as 2 separate chars
                else 
                {
                    str_insert_char(scanner->atr_string,'\\'); //TODO fail check
                    str_insert_char(scanner->atr_string,scanner->curr_char);
                }  
                break;

            case STATE_STR_ESCAPE_HEXA:
                if (
                        (tolower(scanner->curr_char) >= 'a' && tolower(scanner->curr_char) <= 'f') ||
                        (scanner->curr_char >= '0' && scanner->curr_char <= '9')
                    )
                {
                    hexa_number_str[1]=scanner->curr_char; 
                    char* endptr;
                    char result= (char) strtol(hexa_number_str, NULL, 16);  //TODO fail check
                    str_insert_char(scanner->atr_string,result);
                }
                else
                // not valid hexa decimal number, will be writen into resulting string as if there was no escape sequence i.e. one by one
                { 
                    str_insert_char(scanner->atr_string,'\\');
                    str_insert_char(scanner->atr_string,hexa_number_str[0]); //TODO fail check
                    str_insert_char(scanner->atr_string,scanner->curr_char);
                }          
            break;
            
            //case STATE_STRING_END:
        }       
            
    
        

            
    }


}
