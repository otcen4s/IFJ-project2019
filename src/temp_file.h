#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "scanner.h"
#include "symtable.h"
#include "string_lib.h"
#include "error.h"
#include <stdlib.h>

typedef struct
{
    Scanner* scanner;
    Token curr_token;
    tSymbol* global_table;
    tSymbol* local_table;
} Parser;

int start_compiler(char* src_file_name);

// state <stat_list>
// function will expect 
int stat_list(Parser *parser);

int params(Parser *parser)
#endif