#include  "../src/symtable.h"
#include "unity.h"

void setUp(void)
{
    tSymbol *table;
    symtab_init(&table);
}

void tearDown(void)
{
}

void test_symtab_init(void)
{
    tSymbol* table = NULL;
    symtab_init(&table);

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
    symtab_init(&table);
    tSymbol_item *help;

    help = symtab_add(NULL,"1",0); //prazdna tabulka
    TEST_ASSERT_TRUE(strcmp(help->key,"1")==0);
    printf("!!!!!!!!!!!!!!!HELLO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!§");
    //otestovanie INT hodnoty
    help = symtab_add(table,"jedna",0); //pridam jednotku
    TEST_ASSERT_TRUE(strcmp(help->key,"jedna")==0);
    TEST_ASSERT_TRUE(table->size==1);
    symtab_add(table,"dva",0); //pridam jednotku
    symtab_add(table,"tri",0); //pridam jednotku
    TEST_ASSERT_TRUE(table->size==3);

    //Otestovanie double
    help = symtab_add(table,"minus_dva",0); //mala by tam byt -2.4
    TEST_ASSERT_TRUE(help->key==0);
    TEST_ASSERT_TRUE(table->size==4);

    //HLADANIE
    tSymbol_item* search = NULL;
    search = symtab_lookup(table,"jedna",0);
    TEST_ASSERT_NOT_NULL(search);
    search = symtab_lookup(table,"minus_dva",0);
    TEST_ASSERT_NOT_NULL(search);
}

void test_symtab_delete(void)
{
    tSymbol* table = NULL;
    symtab_init(&table); //inicializacia tabulky
    symtab_add(table,"minus_dva",0); //pridanie prvku
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