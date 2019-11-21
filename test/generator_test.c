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
    gen_pushs(token, true);

    //gen_var("dlzka", true);
    //gen_stack("adds");
    
    gen_instruct("ADDS");

    gen_defvar("jej");

    gen_pops("jej", true);
    token.type = TOKEN_IDENTIFIER;
    str_init(&token.attribute.string);
    str_copy(&token.attribute.string, "jej");
    gen_print(1, true, token);

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}