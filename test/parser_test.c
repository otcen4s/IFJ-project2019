#include  "../src/parser.h"
#include "../src/expr_parser.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void print_one_param(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/print_one_param.txt"));
}

void print_multiple_params(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/print_multi_param2"));
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/print_multi_param"));
    
    // multi params with variables
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/print_multi_param_extreme"));
}

void print_errors(void)
{
    //TEST_ASSERT_EQUAL_INT(SYNTAX_ERROR, start_compiler("test/test_data/print_error"));
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/print_error2"));
}

void id_eq_expr(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/id_eq_expr_valid.txt"));
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/id_eq_expr_valid2.txt"));
    TEST_ASSERT_EQUAL_INT(SYNTAX_ERROR, start_compiler("test/test_data/id_eq_expr_invalid1.txt"));
    TEST_ASSERT_EQUAL_INT(SYNTAX_ERROR, start_compiler("test/test_data/id_eq_expr_invalid2.txt"));
    TEST_ASSERT_EQUAL_INT(SYNTAX_ERROR, start_compiler("test/test_data/id_eq_expr_invalid3.txt"));
    
    // multi params with variables
}

void test_inputi(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/inputi_valid1.txt"));
    //TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/inputi_invalid1.txt"));
}

void test_if(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/if_valid_1"));
}

void test_function(void)
{
    TEST_ASSERT_EQUAL_INT(NO_ERROR, start_compiler("test/test_data/function"));
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(print_one_param);
    RUN_TEST(print_multiple_params);
    RUN_TEST(print_errors);
    
    RUN_TEST(id_eq_expr);
    RUN_TEST(test_inputi);
    RUN_TEST(test_if);

    RUN_TEST(test_function);

    return UNITY_END();
}