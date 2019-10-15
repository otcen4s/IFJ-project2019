#ifndef _SCANNER_H
#define _SCANNER_H

#include "enums.h"
#include "string_lib.h"
#include <stdlib.h>
#include <stdio.h>

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
    Scanner_state state;
    tString *atr_string;    
    int is_line_start;
    char curr_char;

} Scanner;

// functions declaration 
Token read_token(int *err);
int init_scanner(Scanner *s, const char* file_name);
static int chceck_keyword(tString* string, Token* token);

#endif
