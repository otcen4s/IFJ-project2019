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

    Token token;
    token.attribute.integer = 3;
    token.type = TOKEN_INTEGER;

    gen_pushs(token);

    token.type = TOKEN_DECIMAL;
    token.attribute.decimal = 5.0;

    gen_pushs(token);

    gen_adds();

    gen_pops("GF@dlzka");

    gen_print("dlzka");

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}