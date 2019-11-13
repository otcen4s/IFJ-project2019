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

void test_symtab_add(void)
{
    // symtab_add(table, "hej", SYMBOL_VAR, DATA_INT, 3)
}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST(test_symtab_add);

    return UNITY_END();
}