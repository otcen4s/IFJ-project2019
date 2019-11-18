#include  "../src/parser.h"
#include "../src/expr_parser.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test1(void)
{
    //TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/parser_test1.txt"));
}


int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}