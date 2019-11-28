
#include  "../src/stack.h"

#include "unity.h"


void setUp(void)
{
}

void tearDown(void)
{
}

// integer stack tests
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

    stack_push(stack, 25);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_push(stack, 9);
    TEST_ASSERT_FALSE(stack_empty(stack));

    int err= NO_ERROR;
    stack_pop(stack, &err);
    
    stack_pop(stack, &err);
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

void test_init(void)
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


// symblol stack tests

void test_symb_stack_empty(void)
{
    t_stack * stack = stack_create(0, SYMBOL_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));

    Symbol s1;
    s1.data_type= 1;
    s1.symbol= 2; 

    Symbol s2;
    s2.data_type= 3;
    s2.symbol= 1; 

    stack_push(stack, s1);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_push(stack, s2);
    TEST_ASSERT_FALSE(stack_empty(stack));

    stack_free(stack);
    stack = stack_create(0, SYMBOL_TYPE);
    TEST_ASSERT_TRUE(stack_empty(stack));
    stack_free(stack);
}

void test_symb_stack_full(void)
{
    t_stack * stack = stack_create(0, SYMBOL_TYPE);

    TEST_ASSERT_TRUE(stack_full(stack));

    Symbol s1;
    s1.data_type= 1;
    s1.symbol= 2; 
    stack_push(stack, s1);

    TEST_ASSERT_FALSE(stack_full(stack));

    stack_free(stack);
    stack = stack_create(2, SYMBOL_TYPE);
    TEST_ASSERT_FALSE(stack_full(stack));
    stack_free(stack);
}

void test_symb_init(void)
{
    int err= 0;
    t_stack * stack = stack_create(0, SYMBOL_TYPE);
    TEST_ASSERT_NOT_NULL(stack);
    
    Symbol s1;
    s1.data_type= 1;
    s1.symbol= 2; 

    stack_push(stack, s1);
    Symbol top = stack_pop(stack, &err).symbol;
    TEST_ASSERT_EQUAL_INT(top.data_type, 1);
    TEST_ASSERT_EQUAL_INT(top.symbol, 2);

    Symbol s2;
    s2.data_type= 5;
    s2.symbol= 1; 

    stack_push(stack, s2);
    top = stack_top(stack ,&err).symbol;
    TEST_ASSERT_EQUAL_INT(top.symbol, 1);
    TEST_ASSERT_EQUAL_INT(top.data_type, 5);

    stack_free(stack);
}


int main(void) 
{
    UNITY_BEGIN();

    //integer stack
    RUN_TEST( test_init);
    RUN_TEST(test_stack_empty);
    RUN_TEST(test_stack_full);
    
    //symbol stack
    RUN_TEST( test_symb_init);
    RUN_TEST(test_symb_stack_empty);
    RUN_TEST(test_symb_stack_full);


    return UNITY_END();
}