
#ifndef "symtable.h"
#define "symtable.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h> //kniznica pre hashovaciu funkciu

#include <symtable.h>

#include "enums.h"
#include "string_lib.h"

/******
* Inicializacia tabulky
*@param h_table tabulka
********/
void hash_table_init (Hash_table *h_table);

/******
* Hashovacia funkcia
*@param str
********/
unsigned int htab_hash_function(const char *str)

/******
* Vkladanie prvkov do tabulky
*@param h_table tabulka
********/
bool hash_table_insertion (Hash_table *h_table, string str);

/******
* Hladanie v tabulke
*@param h_table tabulka
********/
bool hash_table_searching (Hash_table *h_table, string str);

/******
* Uvolnenie tabulky
*@param h_table tabulka
********/
void hash_table_free(Hash_table *h_table);

/******
* Vymazanie prvkov
*@param h_table tabulka
********/
bool hash_table_clear(Hash_table *h_table);

/******
* Vracia "velkost" tabulky 
*@param h_table tabulka
********/
bool hash_table_bucket_count(Hash_table *h_table);

#endif "symtable.h"
