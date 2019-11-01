#define _STACK_TYPE_INT
#include  "../src/stack.h"
#undef _STACK_TYPE_INT

#include "unity.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void test_int(void)
{
    int err= 0;
    t_stack * stack = stack_create(0, INTEGER_TYPE);
    TEST_ASSERT_NOT_NULL(stack);
    stack_push(&err,stack, 25);
    int top = stack_pop(stack, &err).integer;
    TEST_ASSERT_EQUAL_INT(top, 25);


    stack_push(&err,stack, 12);
    top = stack_top(stack ,&err).integer;
    TEST_ASSERT_EQUAL_INT(top, 12);

}

void test_token (void)
{
    int err= 0;
    t_stack * stack = stack_create(0, TOKEN_TYPE);
    TEST_ASSERT_NOT_NULL(stack);
    Token tok; 
    tok.type = 23; 

    stack_push(&err,stack, tok);
    Token top = stack_pop(stack, &err).token;
    
    TEST_ASSERT_EQUAL_INT(top.type, 23);
    top = stack_top(stack ,&err).token; 

}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST( test_int);
    RUN_TEST( test_token);
    return UNITY_END();
}