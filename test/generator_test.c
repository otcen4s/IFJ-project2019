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

    // Token token;
    // token.type = TOKEN_INTEGER;
    // token.attribute.integer = 22;

    // gen_pushs(token, true);

    // token.type = TOKEN_DECIMAL;
    // token.attribute.decimal = 22.0;

    // gen_pushs(token, true);

    // gen_eqs();

    // gen_instruct("POPS GF@$temp");
    // gen_instruct("WRITE GF@$temp");

    gen_instruct("CREATEFRAME");
    gen_instruct("DEFVAR TF@%1");
    gen_instruct("MOVE TF@%1 string@auto");
    gen_instruct("DEFVAR TF@%2");
    gen_instruct("MOVE TF@%2 int@2");
    gen_instruct("DEFVAR TF@%3");
    gen_instruct("MOVE TF@%3 int@3");

    gen_instruct("CALL $substr");

    //gen_instruct("TYPE TF@%retval TF@%retval");
    gen_instruct("WRITE TF@%retval");

    generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}