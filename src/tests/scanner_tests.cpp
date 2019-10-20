#include "gtest/gtest.h"
#include  "../scanner.h"

TEST(scanner_tests, check_keyword)
{
    tString string;
    Token token;
    str_init(&string);
    str_copy(&string, "if");
    check_keyword(&string, &token);

    str_copy(&string, "or");
    check_keyword(&string, &token);
    EXPECT_TRUE( token.type == KEYWORD_OR);

    str_copy(&string, "premenna1");
    check_keyword(&string, &token);
    EXPECT_TRUE( token.type == TOKEN_IDENTIFIER);
}



TEST(scanner_tests, create_int_token)
{
    tString string;
    Token token;
    str_init(&string);

    //corect int
    str_copy(&string, "99");
    int err= 0;    
    token=create_integer_token(string,&err);
    EXPECT_TRUE(token.type == TOKEN_INTEGER);
    EXPECT_TRUE(token.attribute.integer  == 99);
    EXPECT_EQ(err, NO_ERROR);

   //incorect int
    str_copy(&string, "9sfdsf9");
    err= 0;    
    token=create_integer_token(string,&err);
    EXPECT_EQ(err, INTERNAL_ERROR);
}


TEST(scanner_tests, create_decimal_token)
{
    tString string;
    Token token;
    str_init(&string);
    
    //corect decimal
    str_copy(&string, "12.34");
    int err= 0;    
    token=create_decimal_token(string,&err);
    EXPECT_TRUE(token.type == TOKEN_DECIMAL);
    EXPECT_TRUE(token.attribute.decimal  == 12.34);
    EXPECT_EQ(err, NO_ERROR);

   //incorect decimal
    str_copy(&string, "9sfdsf9.32");
    err= 0;    
    token=create_decimal_token(string,&err);
    EXPECT_EQ(err, INTERNAL_ERROR);
}

TEST(scanner_tests, create_str_token)
{
    tString string;
    Token token;
    str_init(&string);
    
    str_copy(&string, "sadaskdoaskd");
    int err= 0;    
    token = create_string_token(string,&err);
    EXPECT_TRUE(token.type == TOKEN_STRING);
    EXPECT_STREQ(token.attribute.string.str, "sadaskdoaskd");
    EXPECT_EQ(err, NO_ERROR);

}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

