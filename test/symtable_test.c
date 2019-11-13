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

   // help = symtab_add(NULL,"1", SYMBOL_VAR, DATA_INT,SYMBOL_DECLARED, 3); //prazdna tabulka
  // TEST_ASSERT_TRUE(help==-1);
    printf("HELLO");
    //otestovanie INT hodnoty
    help = symtab_add(table,"jedna",SYMBOL_VAR, DATA_INT, one, SYMBOL_DECLARED); //pridam jednotku
    TEST_ASSERT_TRUE(help==0);
    //TEST_ASSERT_TRUE(new_symbol->key == "jedna");
    //TEST_ASSERT_TRUE(new_symbol->data_type ==  DATA_INT);
    //TEST_ASSERT_TRUE(new_symbol->symbol_state == SYMBOL_DECLARED);
   // TEST_ASSERT_TRUE(new_symbol->symbol_type == SYMBOL_VAR);
    //TEST_ASSERT_TRUE(new_symbol->symbol_value ==1);

    //Otestovanie FLOAT
    help = symtab_add(table,"minus_dva",SYMBOL_VAR,DATA_DOUBLE,one,SYMBOL_DEFINED); //mala by tam byt -2.4
    TEST_ASSERT_TRUE(help==0);
    //TEST_ASSERT_TRUE(new_symbol->key == "minus_dva");
    //TEST_ASSERT_TRUE(new_symbol->data_type ==  DATA_DOUBLE);
    //TEST_ASSERT_TRUE(new_symbol->symbol_state == SYMBOL_DEFINED);
   // TEST_ASSERT_TRUE(new_symbol->symbol_type == SYMBOL_VAR);
    //TEST_ASSERT_TRUE(new_symbol->symbol_value == -2.4);

    //Otestovanie STRING
    //help = symtab_add(table,"som_string",SYMBOL_VAR,DATA_STRING,one,SYMBOL_DEFINED); //mala by tam byt -2
   // TEST_ASSERT_TRU(help==0);
  //  EXPECT_EQ(new_symbol->key, "som_string");
    //EXPECT_EQ(new_symbol->data_type,  DATA_STRING);
   // EXPECT_EQ(new_symbol->symbol_state,SYMBOL_DEFINED);
   // EXPECT_EQ(new_symbol->symbol_type, SYMBOL_VAR);
    //EXPECT_EQ(new_symbol->symbol_value,"retazec");

    //na malloc testy / iba skuska
   // TEST_ASSERT_TRUE(new_symbol == (sizeof(tSymbol_item)));
  // TEST_ASSERT_TRUE(new_symbol->key== (sizeof(char) * (strlen(key) + 1))); */
}

void test_symtab_delete(void)
{
    tSymbol* table = NULL;
    symtab_init(table);
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