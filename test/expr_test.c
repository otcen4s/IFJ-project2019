#include  "../src/parser.h"
#include  "../src/scanner.h"
#include "../src/expr_parser.h"
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

    //preread token as if it was called from parser
    int err= NO_ERROR; 
    pars->curr_token = read_token(scan, &err);
    TEST_ASSERT_EQUAL_INT(err, NO_ERROR);

    ////////////////////////  ACTUAL TESTING  /////////////////////////////

    TEST_ASSERT_EQUAL_INT(NO_ERROR, expression(pars));

}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(expr_test1);

    return UNITY_END();
}