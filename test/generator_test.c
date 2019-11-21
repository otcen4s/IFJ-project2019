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
    token.attribute.integer = 10;

    gen_pushs(token, true);

    gen_if_start();

    gen_instruct("WRITE string@TRUE\\032");

    str_init(&token.attribute.string);
    str_copy(&token.attribute.string, "Vysla nam nenulova hodnota!");
    token.type = TOKEN_STRING;
    gen_print(1, true, token);

    gen_if_end();

    gen_else_start();

    gen_instruct("WRITE string@FALSE");

    gen_else_end();

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}