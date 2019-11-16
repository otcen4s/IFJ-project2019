#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "scanner.h"
#include "symtable.h"
#include "string_lib.h"
#include "error.h"
#include "function_str.h"
#include <stdlib.h>

typedef struct
{
    Scanner* scanner;
    Token curr_token;
    tSymbol* global_table;
    tSymbol* local_table;
    tSymbol_item* symbol_data;
} Parser;

int start_compiler(char* src_file_name);

#endif