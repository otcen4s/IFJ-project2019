/* IFJ project
**@brief Table of symbols (Symtable)  
**
**@author: Lívia Žitňanská
*/
//TODO THIS CODE IS STILL IN PROCESS AND MANY ERRORS MAY OCCURE (SRRY GUYS :/)

#include "symtable.h"

/**
 * We will be using this hash function  
 */
unsigned int symtab_hash_function(const char *str)
{
    uint32_t h = 0;     // have to be 32-bit
        const unsigned char *p;
        for(p = (const unsigned char*)str; *p != '\0'; p++)
            h = 65599 * h + *p;
        return h;
}

/**
 * Basic initialization of symbol table
*/
int symtab_init(tSymbol *table)
{
    if((table = malloc(sizeof(tSymbol) + SYMTAB_SIZE * sizeof(struct tSymbol_item))) == NULL) {return INTERNAL_ERROR;}
    table->size = 0;
    table->arr_size = SYMTAB_SIZE;

    for(size_t i = 0; i < SYMTAB_SIZE; i++)
    {
        table->item_array[i] = NULL;
    }
    return 0;
}

/**
 * Clears the entire hash table but the allocated structure of size SYMTAB_SIZE stays alive 
*/
//TODO clear is not complete and it will change after the start of syn.analys.(PROBABLY!!)
void symtab_clear(tSymbol *table)
{ 
    for(size_t i = 0; i < table->arr_size; i++)
    {
        struct tSymbol_item *current = table->item_array[i];
        struct tSymbol_item *next = NULL;
        
        while(current != NULL)
        {
            next = current->next_symbol;

            // if it's variable which is string which is defined then call the function to destroy the string
            if((current->symbol_type == SYMBOL_VAR) && (current->data_type == DATA_STRING) && (current->symbol_state == SYMBOL_DEFINED)) 
            {
                str_destroy(&current->symbol_value.string_val);
            }

            free(current->key);
            free(current);
            current = next;
        }
        table->item_array[i] = NULL;
    }
    table->size = 0;
}

/**
 * Frees the entire hash table
*/
void symtab_free(tSymbol *table)
{
    symtab_clear(table);
    free(table);
}

/**
 * Finds and returns the pointer to symbol structure if there is any
 * Uses the key identifier to find the symbol table
*/
tSymbol_item *symtab_lookup(tSymbol *table, const char *key)
{
    if(table == NULL) {return NULL;}

    unsigned int index = (symtab_hash_function(key) % table->arr_size); // generate index to access the right tSymbol_item 
    struct tSymbol_item *new_symbol = table->item_array[index];
    
    while(new_symbol != NULL)
    {
        if(strcmp(key, new_symbol->key) == 0) {return new_symbol;} // check the key identifier if it's what we are looking for
        new_symbol = new_symbol->next_symbol; // go to next symbol
    }
    return NULL;
}

/**
 * In this function we are trying to find the right spot for our new symbol in our symtable
 * If we are lucky we return 0 in other way anything else
*/
int symtab_add(tSymbol *table, const char *key, Symbol_type symbol_type, Data_type data_type, Symbol_value symbol_value, Symbol_state symbol_state)
{
    if(table == NULL) {return -1;}

    unsigned int index = (symtab_hash_function(key) % table->arr_size); 
    struct tSymbol_item *new_symbol = table->item_array[index];
    
    if(new_symbol == NULL) //it's the first symbol on this index
    {
        new_symbol = malloc(sizeof(tSymbol_item));
        new_symbol->key = malloc(sizeof(char) * (strlen(key) + 1));

        if((new_symbol == NULL) || (new_symbol->key == NULL)) {return INTERNAL_ERROR;}

        // bunch of assignes
        strcpy(new_symbol->key, key);
        new_symbol->next_symbol = NULL;
        new_symbol->data_type = data_type;
        new_symbol->symbol_state = symbol_state;
        new_symbol->symbol_type = symbol_type;
        new_symbol->symbol_value = symbol_value;
        
        table->size++;
        table->item_array[index] = new_symbol;
        
        return 0; 
    }

    /**
     * This part says that if the index we are trying to use is occupied we have 
     * to use linked list and find the right spot for our symtable there
    */
    else
    {
        while(new_symbol != NULL) 
        {
            if(strcmp(key, new_symbol->key) == 0) {return -2;} // same key identifier was used, return error
            new_symbol = new_symbol->next_symbol; 
        }
        new_symbol = malloc(sizeof(tSymbol_item));
        new_symbol->key = malloc(sizeof(char) * (strlen(key) + 1));

        if((new_symbol == NULL) || (new_symbol->key == NULL)) {return INTERNAL_ERROR;}

        // bunch of assignes
        strcpy(new_symbol->key, key);
        new_symbol->next_symbol = NULL;
        new_symbol->data_type = data_type;
        new_symbol->symbol_state = symbol_state;
        new_symbol->symbol_type = symbol_type;
        new_symbol->symbol_value = symbol_value; 

        // now we iterate at the end of the linked list and "connect" pointers
        struct tSymbol_item *connect_list = table->item_array[index];
        while(connect_list->next_symbol != NULL)
        {
            connect_list = connect_list->next_symbol;
        }
        connect_list->next_symbol = new_symbol;
        table->size++;

        return 0;
    }
}