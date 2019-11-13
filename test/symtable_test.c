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

void test_symtab_init(void)
{
    tSymbol* table = NULL;
    symtab_init(table);

    TEST_ASSERT_TRUE(table->size == 0);
    TEST_ASSERT_TRUE(table->arr_size == SYMTAB_SIZE);
    TEST_ASSERT_TRUE(table->item_array[1]==NULL);
    TEST_ASSERT_TRUE(table->item_array[9]==NULL);
    TEST_ASSERT_TRUE(table->item_array[0]==NULL);
    TEST_ASSERT_TRUE(table->item_array[8]==NULL);
    TEST_ASSERT_TRUE(table->item_array[3]==NULL);
}
void test_symtab_add(void)
{
    tSymbol* table = NULL;
    symtab_init(table);
    Symbol_value one;
    one.int_val = 1;
    one.double_val = -2.4;
    int help=2;

    help = symtab_add(NULL,"1", SYMBOL_VAR, DATA_INT,one,SYMBOL_DECLARED); //prazdna tabulka
    TEST_ASSERT_TRUE(help==-1);
    printf("HELLO");
    //otestovanie INT hodnoty
    help = symtab_add(table,"jedna",SYMBOL_VAR, DATA_INT, one, SYMBOL_DECLARED); //pridam jednotku
    TEST_ASSERT_TRUE(help==0);
    TEST_ASSERT_TRUE(table->size==1);
    symtab_add(table,"dva",SYMBOL_VAR, DATA_INT, one, SYMBOL_DECLARED); //pridam jednotku
    symtab_add(table,"tri",SYMBOL_VAR, DATA_INT, one, SYMBOL_DECLARED); //pridam jednotku
    TEST_ASSERT_TRUE(table->size==3);

    //Otestovanie double
    help = symtab_add(table,"minus_dva",SYMBOL_VAR,DATA_DOUBLE,one,SYMBOL_DEFINED); //mala by tam byt -2.4
    TEST_ASSERT_TRUE(help==0);
    TEST_ASSERT_TRUE(table->size==4);

    //HLADANIE
    tSymbol* search = NULL;
    search = symtab_lookup(table,"jedna");
    TEST_ASSERT_NOT_NULL(search);
    search = symtab_lookup(table,"minus_dva");
    TEST_ASSERT_NOT_NULL(search);
}

void test_symtab_delete(void)
{
    tSymbol* table = NULL;
    Symbol_value one;
    one.int_val = 1;

    symtab_init(table); //inicializacia tabulky
    symtab_add(table,"minus_dva",SYMBOL_VAR,DATA_INT,one,SYMBOL_DEFINED); //pridanie prvku
    symtab_clear(table);
    TEST_ASSERT_TRUE(table->size==0);
    TEST_ASSERT_TRUE(table->item_array[1]==NULL);
    TEST_ASSERT_TRUE(table->item_array[3]==NULL);
    symtab_free(table);
    TEST_ASSERT_TRUE(table==NULL);
}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST(test_symtab_init);
    RUN_TEST(test_symtab_add);
    RUN_TEST(test_symtab_delete);

    return UNITY_END();
}