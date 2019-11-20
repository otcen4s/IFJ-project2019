
#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "string_lib.h"
#include "function_str.h"
#include "error.h"

#define SYMTAB_SIZE 20047

typedef enum 
{
    SYMBOL_VAR,
    SYMBOL_FUNC,
    SYMBOL_PARAM
} Symbol_type;

typedef enum 
{
    DATA_INT,
    DATA_DOUBLE,
    DATA_STRING,
    DATA_NONE
} Data_type;

typedef enum 
{
    SYMBOL_USED,
    SYMBOL_DEFINED
} Symbol_state;

/*typedef enum
{
    SYMBOL_GLOBAL,
    SYMBOL_LOCAL,
    SYMBOL_BUILT_IN
}Symbol_validity;
*/

/*typedef union 
{
    int int_val;
    double double_val;
    tString string_val;
} Symbol_value;*/

/**
 * This table is the inner one that stores all the data
 * and pointers to next symbol items
 */
//TODO ADD MORE ITEMS
typedef struct tSymbol_item
{
    char *key;
    struct tSymbol_item *next_symbol; // pointer to next table in case of linked list 
    Symbol_type symbol_type;
    Data_type data_type;
    Symbol_state symbol_state;
    int params_count_defined;
    int params_count_used;
    //Symbol_value symbol_value;
    //Symbol_validity symbol_validity;
    //tFunc_params *function_params;
    
}tSymbol_item;

/**
 *  View image at the bottom for description
 */
typedef struct tSymbol
{
    size_t size;
    size_t arr_size;
    tSymbol_item *item_array[]; // pointer to array of structures
}tSymbol;

int symtab_init(tSymbol **table);

void symtab_clear(tSymbol *table);

void symtab_free(tSymbol *table);

unsigned symtab_hash_function(const char *str);

tSymbol_item *symtab_lookup(tSymbol *t, const char *key, int* err);

tSymbol_item* symtab_add(tSymbol *t, const char* key, int* err);


#endif

/*
         THIS IS A VERY LITTLE HELPFUL SKETCH    

  |       +----------+
  |       | size     | // actual number of tSymbol_item [next_sym_item, symbol_type ...]
  |       +----------+
  |       | arr_size | // size of upcoming pointers array 
  |       +----------+
  | |     +---+
  | |     |ptr|-->[next_sym_item, symbol_type ...]-->[next_sym_item, symbol_type ...]-->[next_sym_item, symbol_type ...]--|
  | |     +---+
  | |     |ptr|--| 
  | |     +---+
  | |     |ptr|-->[next_sym_item, symbol_type ...]-->[next_sym_item, symbol_type ...]--|
  | |     +---+
  | |
  | ----> This is an actual hash table construction
  |
  -----> This is a tSymbol table which contains few of a tSymbol_item tabels.
*/