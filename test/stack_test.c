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

void test_stack_empty(void)
{
    int err = 0;

    t_stack * stack = stack_create(0, INTEGER_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));

    stack_push(&err,stack, 25);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_push(&err,stack, 10);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_free(stack);
    stack = stack_create(0, INTEGER_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));
    stack_free(stack);
}

void test_stack_full(void)
{
    int err = 0;
    t_stack * stack = stack_create(0, INTEGER_TYPE);

    TEST_ASSERT_TRUE(stack_full(stack));
    stack_push(&err, stack, 10);
    TEST_ASSERT_FALSE(stack_full(stack));

    stack_free(stack);
    stack = stack_create(2, INTEGER_TYPE);
    TEST_ASSERT_FALSE(stack_full(stack));
    stack_free(stack);
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

    stack_free(stack);

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

    stack_free(stack);

}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST( test_int);
    RUN_TEST( test_token);
    RUN_TEST(test_stack_empty);
    RUN_TEST(test_stack_full);

    return UNITY_END();
}