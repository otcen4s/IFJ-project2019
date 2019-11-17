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
    val1.string = "2";
    val2.string = "5";

    gen_arit("add", TYPE_INT, "dlzka", val1, val2, true);

    gen_var("neviem", TYPE_FLOAT, val1, true);

    gen_print("dlzka");

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}