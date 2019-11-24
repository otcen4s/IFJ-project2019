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
    int help = symtab_init(&table);

    TEST_ASSERT_TRUE(table->size == 0);
    TEST_ASSERT_TRUE(table->arr_size == SYMTAB_SIZE);
    TEST_ASSERT_TRUE(table->item_array[1]==NULL);
    TEST_ASSERT_TRUE(table->item_array[9]==NULL);
    TEST_ASSERT_TRUE(table->item_array[0]==NULL);
    TEST_ASSERT_TRUE(table->item_array[8]==NULL);
    TEST_ASSERT_TRUE(table->item_array[3]==NULL);
    TEST_ASSERT_TRUE(help==0);
}
void test_symtab_add(void)
{
    tSymbol* table = NULL;
    symtab_init(&table);
    tSymbol_item *help;

    //otestovanie ADD
    int err = 1;
    char *key = "jedna";
    help = symtab_add(table,key,&err); //pridam jednotku
    TEST_ASSERT_TRUE(err==0);
    TEST_ASSERT_TRUE(strcmp(help->key,"jedna")==0);
    TEST_ASSERT_TRUE(help->next_symbol==NULL);
    TEST_ASSERT_TRUE(table->size==1);
    char *key2 = "dva";
    help = symtab_add(table,key2,&err); //pridam dvojku
    TEST_ASSERT_TRUE(err==0);
    TEST_ASSERT_TRUE(strcmp(help->key,"dva")==0);
    TEST_ASSERT_TRUE(table->size==2);
    char *key3 = "tri";
    help = symtab_add(table,key3,&err); //pridam dvojku
    TEST_ASSERT_TRUE(err==0);
    TEST_ASSERT_TRUE(strcmp(help->key,"tri")==0);
    TEST_ASSERT_FALSE(table->size==2);
    TEST_ASSERT_TRUE(table->size==3);

    //KONTROLA SPOJENIA
    unsigned int index = (symtab_hash_function(key) % table->arr_size); 
    struct tSymbol_item *spojenie = table->item_array[index];
    //TEST_ASSERT_TRUE(strcmp(spojenie->next_symbol->key,"dva")==0); 
    //index = (symtab_hash_function(key2) % table->arr_size);
    //TEST_ASSERT_TRUE(strcmp(spojenie->next_symbol->key, "tri")==0); 
      
    //HLADANIE
    tSymbol_item* search = NULL;
    search = symtab_lookup(table,key,&err);
    TEST_ASSERT_NOT_NULL(search);
    search = symtab_lookup(table,key2,&err);
    TEST_ASSERT_NOT_NULL(search);
    search = symtab_lookup(table,key3,&err);
    TEST_ASSERT_NOT_NULL(search);
}

void test_symtab_delete(void)
{
    tSymbol* table = NULL;
    symtab_init(&table); //inicializacia tabulky
    int err = 1;
    char *key = "jedna";
    symtab_add(table,key,&err); //pridam jednotku
    TEST_ASSERT_TRUE(table->size==1);
    symtab_clear(table);
    TEST_ASSERT_TRUE(table->size==0);
    TEST_ASSERT_TRUE(table->item_array[1]==NULL);
    TEST_ASSERT_TRUE(table->item_array[3]==NULL);
    symtab_free(table);
    //TEST_ASSERT_TRUE(table==NULL);
}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST(test_symtab_init);
    RUN_TEST(test_symtab_add);
    RUN_TEST(test_symtab_delete);

    return UNITY_END();
}