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
    int init_err = init_scanner(scanner, "test/test_data/scanner_test_1.txt");
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
    init_scanner(scanner, "test/test_data/scanner_test_2.txt");


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
    init_scanner(scanner, "test/test_data/scanner_test_3.txt");


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
    init_scanner(scanner, "test/test_data/scanner_test_4.txt");


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
    init_scanner(scanner, "test/test_data/scanner_test_5.txt");


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
}

void test6_id_or_keyword(void)
{
    /* test file

        if hfd_dssd 

    +

    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_6.txt");


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, KEYWORD_IF);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_IDENTIFIER);
    TEST_ASSERT_EQUAL_STRING(token.attribute.string.str, "hkdfg");
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);        
}

void test7_numbers(void)
{
    /* test file

       923 92.51
       10e5 2E-12 2eh

    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    err= init_scanner(scanner, "test/test_data/scanner_test_7.txt");
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);


    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_INTEGER);
    TEST_ASSERT_EQUAL_INT32(token.attribute.integer, 923);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DECIMAL);
    TEST_ASSERT_EQUAL_FLOAT(token.attribute.decimal, 92.51);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);

    read_token(scanner, &err); // just to get rid of EOL
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DECIMAL);
    TEST_ASSERT_EQUAL_FLOAT(token.attribute.decimal, 10e5);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DECIMAL);
    TEST_ASSERT_EQUAL_INT32(err, NO_ERROR);
    TEST_ASSERT_EQUAL_FLOAT(token.attribute.decimal, 2e-12); 

    token = read_token(scanner, &err); // invalid exponent
    TEST_ASSERT_EQUAL_INT32(err, LEXICAL_ERROR);   
}

void test8_indent(void)
{
/* test file
    +
-
    +
        +
-
    +
  -
*/

    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_8.txt");

    //single indent and dedent
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_INDENT);
    
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_PLUS); 

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL); 

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DEDENT);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_MINUS); 

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);

    // 2 indent levels and dedent back to 0 - generating 2 dedents one after another
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_INDENT);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_PLUS);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_INDENT);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_PLUS);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DEDENT);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_DEDENT);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_MINUS);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);

    //incorrect indent level
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_INDENT);
    
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_PLUS);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL);

    token = read_token(scanner, &err); //here is the incorect indent level
    TEST_ASSERT_EQUAL_INT(err, LEXICAL_ERROR);
}


int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test1_read_token);
    RUN_TEST(test2_token_after_line_commentary);
    RUN_TEST(test3_lesser_eqal_eol_eof);
    RUN_TEST(test4_doscstring);
    RUN_TEST(test5_string);
    RUN_TEST(test6_id_or_keyword);
    RUN_TEST(test7_numbers);
    RUN_TEST(test8_indent);

    return UNITY_END();
}
