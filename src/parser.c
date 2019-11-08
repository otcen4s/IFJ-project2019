#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include "string_lib.h"

int token;
tSymbol* global_table;
tSymbol* local_table;

int counter_var = 1;

void generate_variable_key(tString *var)
{
    //clear
    var->str[0] = '\0';
    var->len = 0;
    str_insert_char(var, '$');

    for(int i = counter_var; i != 0;)
    {
        str_insert_char(var, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counter_var++;
}


