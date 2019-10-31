#include  "../src/scanner.h"
#include "unity.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void test1_read_token(void)
{
    /* test file looks like this
       + *
    */ 
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    int init_err = init_scanner(scanner, "../test/test_data/scanner_test_1.txt");
    TEST_ASSERT_NOT_NULL(scanner->src_file);
    TEST_ASSERT_NOT_NULL(scanner->atr_string); 
    TEST_ASSERT_EQUAL_INT32(init_err, NO_ERROR);
    TEST_ASSERT_TRUE(scanner->is_line_start == 1);
    TEST_ASSERT_EQUAL_INT32(scanner->state, 300);


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_PLUS);

     token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_MULTIPLICATION);
    
}

void test2_token_after_line_commentary(void)
{
    /*test  file looks like this
        # this is line commentary
        =
    */ 
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    int init_err = init_scanner(scanner, "../test/test_data/scanner_test_2.txt");


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL); //commentary should be ignored but EOL shoul be preserved

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_ASSIGN); //next token is =
    
}

void test3_lesser_eqal_eol_eof(void)
{
    /* test file
        <=
    */

    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    int init_err = init_scanner(scanner, "../test/test_data/scanner_test_3.txt");


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_LESSER_EQUAL);
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOF);
    
}

void test4_doscstring(void)
{
    /* test file

       """" this is some 
    docstring  """

    +

    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    int init_err = init_scanner(scanner, "../test/test_data/scanner_test_4.txt");


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);    
}

void test5_string(void)
{
    /* test file:
      'stringliteral'
    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    int init_err = init_scanner(scanner, "../test/test_data/scanner_test_5.txt");


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_STRING(scanner->atr_string->str, "stringliteral");    
    TEST_ASSERT_EQUAL_INT32(token.type , TOKEN_STRING);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type , TOKEN_STRING);
    TEST_ASSERT_EQUAL_STRING(scanner->atr_string->str, "Ahoj\n\"Sve\'te \\\x27"); //example from assignment

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type , TOKEN_EOL);
        
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type , TOKEN_STRING);
    TEST_ASSERT_EQUAL_STRING(scanner->atr_string->str, "\\y"); //not valid escape sequence shoul be reded as 2 characters

    //invalid hexa decimal escape sequence should cause an error
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(err, LEXICAL_ERROR);
    TEST_ASSERT_EQUAL_INT32(token.type , TOKEN_STRING);
}



int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test1_read_token);
    RUN_TEST(test2_token_after_line_commentary);
    RUN_TEST(test3_lesser_eqal_eol_eof);
    RUN_TEST(test4_doscstring);
    RUN_TEST(test5_string);

    return UNITY_END();
}
