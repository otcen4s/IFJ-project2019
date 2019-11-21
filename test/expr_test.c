#include  "../src/parser.h"
#include  "../src/scanner.h"
#include "../src/expr_parser.h"
#include "../src/typedefs.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}


//expression parser test
void expr_test1(void)
{

    //prepare context 
    Parser p;
    Scanner s;
    Parser *pars = &p;
    Scanner *scan =&s;

    init_scanner(scan,"test/test_data/expr_test1.txt");
    init_parser(pars);
    pars->scanner = scan; 

    //preread 2 tokens as if it was called from parser
    int err= NO_ERROR; 
    pars->previous_token = read_token(scan, &err);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR);
    pars->curr_token = read_token(scan, &err);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR);

    ////////////////////////  ACTUAL TESTING  /////////////////////////////

    //init of code generator
    generator_begin();

    TEST_ASSERT_EQUAL_INT(NO_ERROR, expression(pars));

    //print result
    gen_defvar("test_res");
    gen_pops("test_res", true);

    Token print_var;
    print_var.type=TOKEN_IDENTIFIER;
    str_init(&(print_var.attribute.string));
    str_copy(&(print_var.attribute.string), "test_res");

    gen_print(1, true,  print_var);

    FILE *file = fopen("program.code", "w+");
    generate_code(file);
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(expr_test1);

    return UNITY_END();
}