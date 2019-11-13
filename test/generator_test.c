#include  "../src/generator.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test1(void)
{
    FILE *sourceCode;
    
    sourceCode = fopen("program.code", "w+");

    TEST_ASSERT_EQUAL_INT32(NO_ERROR, generator_begin());

    gen_defvar("dlzka");

    Value val1, val2;
    val1.decimal = 10.19392933912193391;
    val2.decimal = 2.332193;

    gen_arit("add", TYPE_FLOAT, "dlzka", val1, val2);

    gen_var("neviem", TYPE_FLOAT, val1);

    gen_print("dlzka");

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}