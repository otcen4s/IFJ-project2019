#include  "../src/symtable.h"
#include "unity.h"

void setUp(void)
{
    tSymbol table;
    symtab_init(&table);
}

void tearDown(void)
{
}

/*int symtab_init(tSymbol *table);
void symtab_clear(tSymbol *table);
void symtab_free(tSymbol *table);
unsigned symtab_hash_function(const char *str);
tSymbol_item *symtab_lookup(tSymbol *t, const char *key);
int symtab_add(tSymbol *t, const char* key, Symbol_type symbol_type, Data_type data_type, Symbol_value symbol_value, Symbol_state symbol_state);
*/
void test_symtab_init(void)
{
    tSymbol table;
    symtab_init(&table);
    EXPECT_EQ(0,table->size);
    *_EQ(table->item_array[1],NULL);
    *_EQ(table->item_array[9],NULL);
    *_EQ(table->item_array[0],NULL);
    *_EQ(table->item_array[8],NULL);
    *_EQ(table->item_array[3],NULL);
}
void test_symtab_add(void)
{
    tSymbol table;
    symtab_init(&table);
    int help=2;

    help = symtab_add(NULL,1, SYMBOL_VAR, DATA_INT,SYMBOL_DECLARED, 3); //prazdna tabulka
    EXPECT_EQ(-1,help);

    //otestovanie INT hodnoty
    help = symtab_add(table,"jedna",SYMBOL_VAR, DATA_INT,SYMBOL_DECLARED,1); //pridam jednotku
    EXPECT_EQ(0,help);
    EXPECT_EQ(new_symbol->key, "jedna");
    EXPECT_EQ(new_symbol->data_type, DATA_INT);
    EXPECT_EQ(new_symbol->symbol_state,SYMBOL_DECLARED);
    EXPECT_EQ(new_symbol->symbol_type, SYMBOL_VAR);
    EXPECT_EQ(new_symbol->symbol_value,1);

    //Otestovanie FLOAT
    help = symtab_add(table,"minus_dva",SYMBOL_VAR,DATA_DOUBLE,SYMBOL_DEFINED,-2.4)); //mala by tam byt -2
    EXPECT_EQ(0,help);
    EXPECT_EQ(new_symbol->key, "minus_dva");
    EXPECT_EQ(new_symbol->data_type,  DATA_DOUBLE);
    EXPECT_EQ(new_symbol->symbol_state,SYMBOL_DEFINED);
    EXPECT_EQ(new_symbol->symbol_type, SYMBOL_VAR);
    EXPECT_EQ(new_symbol->symbol_value,-2.4);

    //Otestovanie STRING
    help = symtab_add(table,"som_string",SYMBOL_VAR,DATA_STRING,SYMBOL_DEFINED,"retazec")); //mala by tam byt -2
    EXPECT_EQ(0,help);
    EXPECT_EQ(new_symbol->key, "som_string");
    EXPECT_EQ(new_symbol->data_type,  DATA_STRING);
    EXPECT_EQ(new_symbol->symbol_state,SYMBOL_DEFINED);
    EXPECT_EQ(new_symbol->symbol_type, SYMBOL_VAR);
    EXPECT_EQ(new_symbol->symbol_value,"retazec");

    //na malloc testy / iba skuska
    EXPECT_EQ(new_symbol,(sizeof(tSymbol_item)));
    EXPECT_EQ(new_symbol->key,(sizeof(char) * (strlen(key) + 1)));
}

void test_symtab_delete(void)
{
    tSymbol table;
    symtab_init(&table);
    test_symtab_delete(table);
    *_EQ(table->size,0);
    *_EQ(table->item_array[1],NULL);
    *_EQ(table->item_array[3],NULL);
    symtab_free(table);
    *_EQ(table,NULL);
}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST(test_symtab_init);
    RUN_TEST(test_symtab_add);
    RUN_TEST(test_symtab_delete);

    return UNITY_END();