#include "string_lib.h"
#include "scanner.h"
#include "error.h"
#include  "string_lib.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h> 

// create and init scanner
Scanner *scanner;

int init_scanner(Scanner *s, const char* file_name)
{ 
    if (s->src_file=fopen(file_name, "r")) return INTERNAL_ERROR;
    s->is_line_start= 1;
    s->state= STATE_START;
    s->curr_char=0; 
    s->atr_string= NULL; 
}

static int chceck_keyword(tString* string, Token* token){
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
    char *endptr = 0;

    int value = strtol(string.str, endptr, 10);

    if (*endptr) {
        *error = INTERNAL_ERROR;
        return;
    }

    token.type = TOKEN_INTEGER;
    token.attribute.integer = value;
    
    return token;
}

Token create_decimal_token(tString string, int *error) {
    *error = NO_ERROR;
    
    Token token;
    char *endptr = 0;

    double value = strtod(string.str, endptr);

    if (*endptr) {
        *error = INTERNAL_ERROR;
        return;
    }

    token.type = TOKEN_DECIMAL;
    token.attribute.decimal = value;
    
    return token;
}

Token create_string_token(char *string, int *error) {
    *error = NO_ERROR;
    
    Token token;

    token.type = TOKEN_STRING;
    *error = str_copy(&token.attribute.string, string);
    
    return token;
}

Token read_token(int *err)
{
    //init atribute string
    if(str_init(scanner->atr_string)) return INTERNAL_ERROR;


    while(true)
    {
       //read character form file
       scanner->curr_char=getc(scanner->src_file);
       Token  token;
  


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
            else if (scanner->curr_char == '\n-' ) scanner->state = STATE_EOL;
            else if (scanner->curr_char == ',') scanner->state = STATE_COMMA;
            else if (scanner->curr_char == '#') scanner->state = STATE_HASH;
            else if (isalpha(scanner->curr_char) || scanner->curr_char == '_' ) scanner->state = STATE_ID;
            else if (scanner->curr_char == '#') scanner->state = STATE_HASH;
            else if (scanner->curr_char == '"') scanner->state = STATE_STRING_START;
            else scanner->state = STATE_ERROR; //
            

            break;
        
        case STATE_PLUS:
            token.type=TOKEN_PLUS;
        
        

        
            

            break;
        

        }
    }


}
