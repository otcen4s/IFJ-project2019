#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "scanner.h"
#include "symtable.h"
#include "string_lib.h"
#include "error.h"
#include "function_str.h"
#include <stdlib.h>
#include <stdbool.h>
#include "expr_parser.h"

typedef struct
{
    Scanner* scanner;
    Token curr_token;
    Token previous_token; // this is for expression parser
    tSymbol* global_table;
    tSymbol* local_table;
    tSymbol_item* symbol_data_global;
    tSymbol_item* symbol_data_local; 
    tSymbol_item* left_side; // for storing current left hand side identifier 
    bool is_in_if;
    bool if_expression;
    bool is_in_while;
    bool while_expression;
    bool is_in_def;
    bool is_in_print;
    bool no_assign_expression;
    bool is_in_return;
    bool expr_parser_call;
    unsigned nested_cnt;
} Parser;

int start_compiler(char* src_file_name);

//predeclaratins, just for testing reasons
int init_parser(Parser* parser);

#endif