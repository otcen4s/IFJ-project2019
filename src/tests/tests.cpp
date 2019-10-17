#include "gtest/gtest.h"
#include "../string_lib.c"

tString string;

TEST(string_lib, str_copy){
    
    str_copy(&string, "ahoj");
    EXPECT_STREQ(string.str,"ahoj");
    EXPECT_EQ(string.len, 5);
   
}
TEST(string_lib, str_copy_arr_size){
    str_realloc(&string, 10);
    EXPECT_EQ(string.arr_size,128+10);
    EXPECT_NE(string.arr_size, 139);
}
TEST(string_lib, str_cmp_keyword){
    str_copy(&string, "for");
    EXPECT_TRUE(str_cmp_keyword(&string, "for"));
    str_copy(&string, "rof");
    EXPECT_FALSE(str_cmp_keyword(&string, "for"));
}

int main(int argc, char **argv) {
    str_init(&string);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}