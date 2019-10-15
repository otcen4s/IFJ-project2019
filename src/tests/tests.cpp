#include "gtest/gtest.h"

#include "../string_lib.h"

tString string;

TEST(stringLib, strCopy)
{
    str_copy(&string, "ahoj");
    EXPECT_EQ(string.str, "ahoj");	
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}