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
    TEST_ASSERT_EQUAL_INT32(string.len, 4);
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

void test_str_insert_char(void) {
    tString string;
    str_init(&string);
    str_copy(&string, "test");
    str_insert_char(&string, 'a');
    TEST_ASSERT_EQUAL_STRING("testa", string.str);
}

void test_str_destroy(void) {
    tString string;
    str_init(&string);
    str_copy(&string, "test");

    str_destroy(&string);

    TEST_ASSERT_EQUAL_STRING("", string.str);
}

void test_str_init(void) {
    tString string;
    str_init(&string);

    TEST_ASSERT_EQUAL_STRING("", string.str);
    TEST_ASSERT_EQUAL_INT32(0, string.len);
    TEST_ASSERT_EQUAL_INT32(64, string.arr_size);

    str_copy(&string, "test");
    str_init(&string);

    TEST_ASSERT_EQUAL_STRING("", string.str);
    TEST_ASSERT_EQUAL_INT32(0, string.len);
    TEST_ASSERT_EQUAL_INT32(64, string.arr_size);

}

void test_str_realloc(void) {
    tString string;
    str_init(&string);

    str_copy(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    TEST_ASSERT_EQUAL_INT32(651, string.len);
    TEST_ASSERT_EQUAL_INT32(651 + 128, string.arr_size);
    TEST_ASSERT_EQUAL_STRING("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", string.str);

    str_realloc(&string, 1000);
    TEST_ASSERT_EQUAL_INT32(651, string.len);
    TEST_ASSERT_EQUAL_INT32(1128, string.arr_size);

    str_copy(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    TEST_ASSERT_EQUAL_INT32(1000, string.len);
    TEST_ASSERT_EQUAL_INT32(1128, string.arr_size);
    TEST_ASSERT_EQUAL_STRING("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", string.str);

}

void test_str_append(void) {
    tString string;
    str_init(&string);

    str_append(&string, "auto");
    TEST_ASSERT_EQUAL_STRING("auto", string.str);

    str_append(&string, "bus");
    TEST_ASSERT_EQUAL_STRING("autobus", string.str);

}

void test_str_concat(void) {
    tString string;
    str_init(&string);

    str_concat(&string, "Volam ", "sa ", "auto", "bus", NULL);
    TEST_ASSERT_EQUAL_STRING("Volam sa autobus", string.str);

    str_concat(&string, "Ako ", "sa ", "mas?", NULL);
    TEST_ASSERT_EQUAL_STRING("Ako sa mas?", string.str);
}

int main(void) 
{
    UNITY_BEGIN();

    RUN_TEST(test_str_cpy);
    RUN_TEST(test_str_copy_arr_size);
    RUN_TEST(test_str_cmp_keyword);
    RUN_TEST(test_str_insert_char);
    RUN_TEST(test_str_realloc);
    RUN_TEST(test_str_destroy);
    RUN_TEST(test_str_init);
    RUN_TEST(test_str_append);
    RUN_TEST(test_str_concat);

    return UNITY_END();
}
