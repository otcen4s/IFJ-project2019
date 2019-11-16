#ifndef _SCANNER_H
#define _SCANNER_H

#include "stack.h"
#include "enums.h"
#include "error.h"
#include  "string_lib.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 

#define MAX_UNGETED_TOKENS 100

typedef union {
	tString string;
	int integer;
	double decimal;
} Token_attribute;

typedef struct {
	Token_type type;
	Token_attribute attribute;
} Token;

typedef struct {
    FILE *src_file;
    int state;
    struct stack_struct *stack;
    tString *atr_string;    
    int is_line_start;
    char curr_char;
    int indent_cnt;
} Scanner;

// functions forward declarations
Token create_string_token(tString string);
Token create_decimal_token(tString string);
Token create_integer_token(tString string, int *error);
void check_keyword(tString* string, Token* token);
Token read_token(Scanner *scanner, int *err);
int init_scanner(Scanner *s, const char* file_name);

#endif
