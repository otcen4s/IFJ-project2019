#include  "../src/string_lib.h"
#include "unity.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void test_str_cpy(void)
{
    tString string;
    str_init(&string);
    str_copy(&string, "ahoj");
    TEST_ASSERT_EQUAL_STRING("ahoj",string.str);
    TEST_ASSERT_EQUAL_INT32(string.len, 5);
}

void test_str_copy_arr_size(void)
{
   tString string;
   str_init(&string);
   str_realloc(&string, 10);
   TEST_ASSERT_EQUAL_INT32(string.arr_size,128+10);
}


void test_str_cmp_keyword(void)
{
    tString string;
    str_init(&string);
    str_copy(&string, "for");
    TEST_ASSERT_TRUE(str_cmp_keyword(&string, "for"));
    str_copy(&string, "rof");
    TEST_ASSERT_FALSE(str_cmp_keyword(&string, "for"));
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_str_cpy);
    RUN_TEST(test_str_copy_arr_size);
    RUN_TEST(test_str_cmp_keyword);

    return UNITY_END();
}
