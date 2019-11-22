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

    Token token;
    token.type = TOKEN_INTEGER;
    token.attribute.integer = 22;

    gen_pushs(token, true);

    token.type = TOKEN_DECIMAL;
    token.attribute.decimal = 22.0;

    gen_pushs(token, true);

    gen_eqs();

    gen_instruct("POPS GF@$temp");
    gen_instruct("WRITE GF@$temp");

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}