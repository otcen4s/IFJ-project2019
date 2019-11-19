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
    token.type = TOKEN_IDENTIFIER;
    str_init(&(token.attribute.string));
    str_copy(&(token.attribute.string), "dlzka");

    Token token2;
    token2.type = TOKEN_INTEGER;
    token2.attribute.integer = 69;

    gen_print(2, true, token, token2);

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}