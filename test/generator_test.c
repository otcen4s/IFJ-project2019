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
    generator_begin();
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(test1);

    return UNITY_END();
}