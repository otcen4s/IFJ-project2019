#include "string_lib.h"
#include "scanner.h"
#include "error.h"
#include  "string_lib.h"
#include <stdbool.h>

// create and init scanner
Scanner *scanner;
init_scanner(scanner);

int init_scanner(Scanner *s, const char* file_name)
{ 
    if (s->src_file=fopen(file_name, "r")) return INTERNAL_ERROR;
    s->is_line_start= 1;
    s->state= STATE_START;
    s->curr_char=0; 
    str_init(s->atr_string);    
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

Token read_token()
{
    while(true)
    {
       //read character form file
       scanner->curr_char=getc(scanner->src_file);


        switch (scanner->state)
        {
        case STATE_START:            
            if(scanner->curr_char == '+') scanner->state = STATE_PLUS; // + 
            else if (scanner->curr_char == '-') scanner->state = STATE_MINUS; // -
            else if (scanner->curr_char == '(')


            break;
        

        }
    }


}
