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
    t_stack * stack = stack_create(0, INTEGER_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));

    stack_push(stack, 25);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_push(stack, 10);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_free(stack);
    stack = stack_create(0, INTEGER_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));
    stack_free(stack);
}

void test_stack_full(void)
{
    t_stack * stack = stack_create(0, INTEGER_TYPE);

    TEST_ASSERT_TRUE(stack_full(stack));
    stack_push(stack, 10);
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
    stack_push(stack, 25);
    int top = stack_pop(stack, &err).integer;
    TEST_ASSERT_EQUAL_INT(top, 25);


    stack_push(stack, 12);
    top = stack_top(stack ,&err).integer;
    TEST_ASSERT_EQUAL_INT(top, 12);

    stack_free(stack);

}


//add tests for other types here

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST( test_int);
    RUN_TEST(test_stack_empty);
    RUN_TEST(test_stack_full);

    return UNITY_END();
}