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
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL); //firs toknen EOL

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_ASSIGN); //next token is =

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(token.type, TOKEN_EOL); //next token eof
    
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
    TEST_ASSERT_EQUAL_INT(TOKEN_STRING, token.type);
    TEST_ASSERT_EQUAL_STRING("this is first doc st\"\"ring",token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(TOKEN_EOL, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(token.type, TOKEN_STRING);
    TEST_ASSERT_EQUAL_STRING("first line \"\nsecond line",token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR); 

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(TOKEN_EOL, token.type);  

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(TOKEN_IDENTIFIER, token.type); 

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(TOKEN_ASSIGN, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT(token.type, TOKEN_STRING);
    TEST_ASSERT_EQUAL_STRING("some string",token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR);
    
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

void test9_function(void)
{
/* test file
def my_function():
    print("Mam 42 rokov")
    vek = 42.56 
*/

    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_9.txt");

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(KEYWORD_DEF, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_STRING("my_function", token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT32(11, token.attribute.string.len);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LEFT_BRACKET, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_RIGHT_BRACKET, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_COLON, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EOL, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INDENT, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(KEYWORD_PRINT, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LEFT_BRACKET, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STRING, token.type);
    TEST_ASSERT_EQUAL_STRING("Mam 42 rokov", token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT32(12, token.attribute.string.len);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_RIGHT_BRACKET, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EOL, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_STRING("vek", token.attribute.string.str);
    TEST_ASSERT_EQUAL_INT32(3, token.attribute.string.len);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_ASSIGN, token.type);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_DECIMAL, token.type);
    TEST_ASSERT_EQUAL_FLOAT(42.56, token.attribute.decimal);

    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EOF, token.type);

}

void test10_docstring_escape_seq(void)
{
    /* test file

    """some multi line 
    \"""continues \""""

    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_10.txt");
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(NO_ERROR, err);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STRING, token.type);
    TEST_ASSERT_EQUAL_STRING( " sadsad \" \"\"\ncontinue sasasdasd \"\"\" ",token.attribute.string.str);
}

void test12_bad_double(void)
{
    /* test file

    2.54Z3

    */
    
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_12.txt");
    token = read_token(scanner, &err);
    TEST_ASSERT_EQUAL_INT32(LEXICAL_ERROR, err);
}

void test11_error_indentation(void) {
    int err = 0; 
    Token token; 
    Scanner s; 
    Scanner *scanner = &s; 
    init_scanner(scanner, "test/test_data/scanner_test_8.txt");
    token = read_token(scanner, &err);

    while (token.type != TOKEN_EOF) {
        token = read_token(scanner, &err);
        
        if (err) {
            break;
        }
    }

    TEST_ASSERT_EQUAL_INT(LEXICAL_ERROR, err);
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
    RUN_TEST(test9_function);
    RUN_TEST(test10_docstring_escape_seq);
    RUN_TEST(test11_error_indentation);
   // RUN_TEST(test12_bad_double);

    return UNITY_END();
}
