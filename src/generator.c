#include "generator.h"

#define ADDLINE(line) \
do { \
str_append(&tempCode, line); \
str_append(&tempCode, "\n"); \
 } while (0) \

#define ADDCODE(line) \
str_append(&tempCode, line); \

#define ISGLOBAL(global) \
global ? "GF@" : "LF@" \

tString code;
tString line;
tString helper;
tString tempCode;
tString currFuncName;

int error;
int paramCounter = 0;
int defParamCounter = 0;
int ifUID = 0;
int whileUID = 0;
char uidStr[100];
t_stack * ifStack;
t_stack * whileStack;

int generator_begin() {
    if ((error = str_init(&code))) {
        return error;
    }

    if ((error = str_init(&line))) {
        return error;
    }

    if ((error = str_init(&helper))) {
        return error;
    }

    if ((error = str_init(&tempCode))) {
        return error;
    }

    if ((error = str_init(&currFuncName))) {
        return error;
    }

    ifStack = stack_create(0, INTEGER_TYPE);
    whileStack = stack_create(0, INTEGER_TYPE);

    // generate header
    ADDLINE(".IFJcode19");

    // TODO defining global variables to use in functions, maybe not the best approach
    ADDLINE("DEFVAR GF@$op1");
    ADDLINE("DEFVAR GF@$op2");
    ADDLINE("DEFVAR GF@$op1Type");
    ADDLINE("DEFVAR GF@$op2Type");
    ADDLINE("DEFVAR GF@$temp");
    
    ADDLINE("JUMP $$main");
    
    ADDLINE("LABEL $error");
    ADDLINE("EXIT int@4");

    ADDLINE("LABEL $divisionByZero");
    ADDLINE("EXIT int@9");
    
    // defining helper function for print
    ADDLINE("LABEL $print");
    ADDLINE("TYPE GF@$op1Type GF@$op1");
    ADDLINE("JUMPIFNEQ $printEnd GF@$op1Type string@nil");
    ADDLINE("WRITE string@None");
    ADDLINE("RETURN");

    ADDLINE("LABEL $printEnd");
    ADDLINE("WRITE GF@$op1");
    ADDLINE("RETURN");


    // defining function adds
    ADDLINE("LABEL $adds");

    ADDLINE("JUMPIFEQ $addsSameType GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $addsOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $addsOp1Float GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $addsOp1Int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $addsAddFinish");

    ADDLINE("LABEL $addsOp1Float");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $addsAddFinish");

    ADDLINE("LABEL $addsSameType");
    ADDLINE("JUMPIFEQ $addsAddFinish GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $addsAddFinish GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $addsConcatFinish GF@$op1Type string@string");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $addsAddFinish");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("ADDS");
    ADDLINE("RETURN");

    ADDLINE("LABEL $addsConcatFinish");
    ADDLINE("CONCAT GF@$temp GF@$op1 GF@$op2");
    ADDLINE("PUSHS GF@$temp");
    ADDLINE("RETURN");
    
    // defining function subs
    ADDLINE("LABEL $subs");

    ADDLINE("JUMPIFEQ $subsSameType GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $subsOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $subsOp1Float GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $subsOp1Int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $subsFinish");

    ADDLINE("LABEL $subsOp1Float");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $subsFinish");

    ADDLINE("LABEL $subsSameType");
    ADDLINE("JUMPIFEQ $subsFinish GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $subsFinish GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $subsFinish");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("SUBS");
    ADDLINE("RETURN");

    // defining function muls
    ADDLINE("LABEL $muls");

    ADDLINE("JUMPIFEQ $mulsSameType GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $mulsOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $mulsOp1Float GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $mulsOp1Int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $mulsFinish");

    ADDLINE("LABEL $mulsOp1Float");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $mulsFinish");

    ADDLINE("LABEL $mulsSameType");
    ADDLINE("JUMPIFEQ $mulsFinish GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $mulsFinish GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $mulsFinish");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("MULS");
    ADDLINE("RETURN");

    // defining function divs
    ADDLINE("LABEL $divs");

    ADDLINE("JUMPIFEQ $divsSameType GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $divsOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $divsOp1Float GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $divsOp1Int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $divsFinish");

    ADDLINE("LABEL $divsOp1Float");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $divsFinish");

    ADDLINE("LABEL $divsSameType");
    ADDLINE("JUMPIFEQ $divsFinish GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $divsSameTypeInt GF@$op1Type string@int");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $divsSameTypeInt");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $divsFinish");

    ADDLINE("LABEL $divsFinish");
    ADDLINE("JUMPIFEQ $divisionByZero GF@$op2 float@0x0p+0");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("DIVS");
    ADDLINE("RETURN");

    // defining function idivs
    ADDLINE("LABEL $idivs");

    ADDLINE("JUMPIFNEQ $error GF@$op1Type string@int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("JUMPIFEQ $divisionByZero GF@$op2 int@0");

    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("IDIVS");
    ADDLINE("RETURN");

    // evaluate function definition - used in if and while
    ADDLINE("LABEL $eval");
    ADDLINE("POPS GF@$op1");
    ADDLINE("TYPE GF@$op1Type GF@$op1");

    ADDLINE("JUMPIFEQ $evalInt GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $evalFloat GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $evalString GF@$op1Type string@string");
    ADDLINE("JUMPIFEQ $evalFalse GF@$op1Type string@nil");
    ADDLINE("JUMPIFEQ $evalBool GF@$op1Type string@bool");

    ADDLINE("LABEL $evalInt");
    ADDLINE("JUMPIFEQ $evalFalse GF@$op1 int@0");
    ADDLINE("JUMP $evalTrue");

    ADDLINE("LABEL $evalFloat");
    ADDLINE("JUMPIFEQ $evalFalse GF@$op1 float@0x0p+0");
    ADDLINE("JUMP $evalTrue");

    ADDLINE("LABEL $evalString");
    ADDLINE("JUMPIFEQ $evalFalse GF@$op1 string@");
    ADDLINE("JUMP $evalTrue");

    ADDLINE("LABEL $evalBool");
    ADDLINE("JUMPIFEQ $evalFalse GF@$op1 bool@false");
    ADDLINE("JUMP $evalTrue");

    ADDLINE("LABEL $evalTrue");
    ADDLINE("MOVE GF@$temp bool@true");
    ADDLINE("RETURN");

    ADDLINE("LABEL $evalFalse");
    ADDLINE("MOVE GF@$temp bool@false");
    ADDLINE("RETURN");

    // function substr
    ADDLINE("LABEL $substr");
    ADDLINE("PUSHFRAME");

    ADDLINE("TYPE GF@$temp LF@%1");
    ADDLINE("JUMPIFNEQ $error GF@$temp string@string");

    ADDLINE("TYPE GF@$temp LF@%2");
    ADDLINE("JUMPIFNEQ $error GF@$temp string@int");

    ADDLINE("TYPE GF@$temp LF@%3");
    ADDLINE("JUMPIFNEQ $error GF@$temp string@int");

    ADDLINE("DEFVAR LF@%retval");
	ADDLINE("MOVE LF@%retval string@");

    ADDLINE("DEFVAR LF@str_length");
    ADDLINE("STRLEN LF@str_length LF@%1");

    ADDLINE("DEFVAR LF@cond");

    ADDLINE("LT LF@cond LF@%2 int@0");
    ADDLINE("JUMPIFEQ $substrNone LF@cond bool@true");

    ADDLINE("GT LF@cond LF@%2 LF@str_length");
    ADDLINE("JUMPIFEQ $substrNone LF@cond bool@true");

    ADDLINE("LT LF@cond LF@%3 int@0");
    ADDLINE("JUMPIFEQ $substrNone LF@cond bool@true");

    ADDLINE("LT LF@cond LF@str_length int@1");
    ADDLINE("JUMPIFEQ $substrReturn LF@cond bool@true");

    ADDLINE("JUMPIFEQ $substrReturn LF@%3 int@0");

    ADDLINE("DEFVAR LF@letter");
    ADDLINE("DEFVAR LF@index");
    ADDLINE("MOVE LF@index LF@%2");

    ADDLINE("SUB LF@str_length LF@str_length int@1");
    ADDLINE("LABEL $substrLoop");
    ADDLINE("GT LF@cond LF@index LF@str_length");
    ADDLINE("JUMPIFEQ $substrReturn LF@cond bool@true");

    ADDLINE("GETCHAR LF@letter LF@%1 LF@index");
    ADDLINE("CONCAT LF@%retval LF@%retval LF@letter");
    ADDLINE("ADD LF@index LF@index int@1");
    ADDLINE("SUB LF@%3 LF@%3 int@1");

    ADDLINE("JUMPIFNEQ $substrLoop LF@%3 int@0");
    ADDLINE("JUMP $substrReturn");

    ADDLINE("LABEL $substrNone");
    ADDLINE("MOVE LF@%retval nil@nil");

    ADDLINE("LABEL $substrReturn");
	ADDLINE("POPFRAME");
	ADDLINE("RETURN");
    

    // function len
    ADDLINE("LABEL $len");
    ADDLINE("PUSHFRAME");
    ADDLINE("TYPE GF@$temp LF@%1");
    ADDLINE("JUMPIFNEQ $error GF@$temp string@string");
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("STRLEN LF@%retval LF@%1");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function chr
    ADDLINE("LABEL $chr");
    ADDLINE("PUSHFRAME");
    ADDLINE("DEFVAR LF@%retval");

    ADDLINE("TYPE GF@$temp LF@%1");
    ADDLINE("JUMPIFNEQ $error GF@$temp string@int");

    ADDLINE("LT GF@$temp LF@%1 int@0");
    ADDLINE("JUMPIFEQ $error GF@$temp bool@true");

    ADDLINE("GT GF@$temp LF@%1 int@255");
    ADDLINE("JUMPIFEQ $error GF@$temp bool@true");

    ADDLINE("INT2CHAR LF@%retval LF@%1");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function ord
    ADDLINE("LABEL $ord");
    ADDLINE("PUSHFRAME");
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("DEFVAR LF@cond");
    ADDLINE("DEFVAR LF@len");
    ADDLINE("DEFVAR LF@type");

    ADDLINE("TYPE LF@type LF@%1");
    ADDLINE("JUMPIFNEQ $error LF@type string@string");

    ADDLINE("TYPE LF@type LF@%2");
    ADDLINE("JUMPIFNEQ $error LF@type string@int");

    ADDLINE("STRLEN LF@len LF@%1");
    ADDLINE("SUB LF@len LF@len int@1");
    ADDLINE("GT LF@cond LF@%2 LF@len");
    ADDLINE("JUMPIFEQ $ordNone LF@cond bool@true");

    ADDLINE("LT LF@cond LF@%2 int@0");
    ADDLINE("JUMPIFEQ $ordNone LF@cond bool@true");

    ADDLINE("STRI2INT LF@%retval LF@%1 LF@%2");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    ADDLINE("LABEL $ordNone");
    ADDLINE("MOVE LF@%retval nil@nil");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function inputs
    ADDLINE("LABEL $inputs");
    ADDLINE("PUSHFRAME");
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("READ LF@%retval string");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function inputi
    ADDLINE("LABEL $inputi");
    ADDLINE("PUSHFRAME");
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("READ LF@%retval int");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function inputf
    ADDLINE("LABEL $inputf");
    ADDLINE("PUSHFRAME");
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("READ LF@%retval float");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function greater than or equal
    ADDLINE("LABEL $gtes");
    ADDLINE("POPS GF@$op2");
    ADDLINE("POPS GF@$op1");

    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");

    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");

    ADDLINE("CALL $eq");
    ADDLINE("PUSHS bool@true");
    ADDLINE("JUMPIFEQS $gtesTrue");

    ADDLINE("CALL $glt");
    ADDLINE("GTS");
    ADDLINE("RETURN");

    ADDLINE("LABEL $gtesTrue");
    ADDLINE("PUSHS bool@true");
    ADDLINE("RETURN");

    // function lesser than or equal
    ADDLINE("LABEL $ltes");
    ADDLINE("POPS GF@$op2");
    ADDLINE("POPS GF@$op1");

    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");

    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");

    ADDLINE("CALL $eq");
    ADDLINE("PUSHS bool@true");
    ADDLINE("JUMPIFEQS $ltesTrue");

    ADDLINE("CALL $glt");
    ADDLINE("LTS");
    ADDLINE("RETURN");

    ADDLINE("LABEL $ltesTrue");
    ADDLINE("PUSHS bool@true");
    ADDLINE("RETURN");

    // lesser than, greater than functions
    ADDLINE("LABEL $glt");
    ADDLINE("POPS GF@$op2");
    ADDLINE("POPS GF@$op1");
    ADDLINE("TYPE GF@$op1Type GF@$op1");
    ADDLINE("TYPE GF@$op2Type GF@$op2");

    ADDLINE("JUMPIFEQ $gltFinish GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $gltOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $gltOp1Float GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $gltOp1Bool GF@$op1Type string@bool");
    ADDLINE("EXIT int@4");

    ADDLINE("LABEL $gltOp1Int");
    ADDLINE("JUMPIFEQ $gltOp1IntOp2Float GF@$op2Type string@float");
    ADDLINE("JUMPIFEQ $gltOp1IntOp2Bool GF@$op2Type string@bool");
    ADDLINE("EXIT int@4");

    ADDLINE("LABEL $gltOp1IntOp2Float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltOp1IntOp2Bool");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 int@1");
    ADDLINE("JUMPIFEQ $gltFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op2 int@0");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltOp1Float");
    ADDLINE("JUMPIFEQ $gltOp1FloatOp2Int GF@$op2Type string@int");
    ADDLINE("JUMPIFEQ $gltOp1FloatOp2Bool GF@$op2Type string@bool");
    ADDLINE("EXIT int@4");

    ADDLINE("LABEL $gltOp1FloatOp2Int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltOp1FloatOp2Bool");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 float@0x1p+0");
    ADDLINE("JUMPIFEQ $gltFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op2 float@0x0p+0");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltOp1Bool");
    ADDLINE("JUMPIFEQ $gltOp1BoolOp2Int GF@$op2Type string@int");
    ADDLINE("JUMPIFEQ $gltOp1BoolOp2Float GF@$op2Type string@float");
    ADDLINE("EXIT int@4");

    ADDLINE("LABEL $gltOp1BoolOp2Int");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 bool@true");
    ADDLINE("JUMPIFEQ $gltFinish GF@$temp int@1");
    ADDLINE("MOVE GF@$op2 bool@false");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltOp1BoolOp2Float");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 bool@true");
    ADDLINE("JUMPIFEQ $gltFinish GF@$temp float@0x1p+0");
    ADDLINE("MOVE GF@$op2 bool@false");
    ADDLINE("JUMP $gltFinish");

    ADDLINE("LABEL $gltFinish");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("RETURN");

    // not equal function
    ADDLINE("LABEL $neq");
    ADDLINE("CALL $eq");
    ADDLINE("NOTS");
    ADDLINE("RETURN");

    // equal function
    ADDLINE("LABEL $eq");
    ADDLINE("POPS GF@$op2");
    ADDLINE("POPS GF@$op1");
    ADDLINE("TYPE GF@$op1Type GF@$op1");
    ADDLINE("TYPE GF@$op2Type GF@$op2");

    ADDLINE("JUMPIFEQ $eqFinish GF@$op1Type GF@$op2Type");

    ADDLINE("JUMPIFEQ $eqFinish GF@$op1Type string@nil");
    ADDLINE("JUMPIFEQ $eqFinish GF@$op2Type string@nil");

    ADDLINE("JUMPIFEQ $eqNotEqual GF@$op1Type string@string");
    ADDLINE("JUMPIFEQ $eqNotEqual GF@$op2Type string@string");

    ADDLINE("JUMPIFEQ $eqOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $eqOp1Float GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $eqOp1Bool GF@$op1Type string@bool");

    ADDLINE("LABEL $eqOp1Int");
    ADDLINE("JUMPIFEQ $eqOp1IntOp2Float GF@$op2Type string@float");
    ADDLINE("JUMPIFEQ $eqOp1IntOp2Bool GF@$op2Type string@bool");

    ADDLINE("LABEL $eqOp1IntOp2Float");
    ADDLINE("INT2FLOAT GF@$op1 GF@$op1");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqOp1IntOp2Bool");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 int@1");
    ADDLINE("JUMPIFEQ $eqFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op2 int@0");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqOp1Float");
    ADDLINE("JUMPIFEQ $eqOp1FloatOp2Int GF@$op2Type string@int");
    ADDLINE("JUMPIFEQ $eqOp1FloatOp2Bool GF@$op2Type string@bool");

    ADDLINE("LABEL $eqOp1FloatOp2Int");
    ADDLINE("INT2FLOAT GF@$op2 GF@$op2");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqOp1FloatOp2Bool");
    ADDLINE("MOVE GF@$temp GF@$op2");
    ADDLINE("MOVE GF@$op2 float@0x1p+0");
    ADDLINE("JUMPIFEQ $eqFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op2 float@0x0p+0");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqOp1Bool");
    ADDLINE("JUMPIFEQ $eqOp1BoolOp2Int GF@$op2Type string@int");
    ADDLINE("JUMPIFEQ $eqOp1BoolOp2Float GF@$op2Type string@float");

    ADDLINE("LABEL $eqOp1BoolOp2Int");
    ADDLINE("MOVE GF@$temp GF@$op1");
    ADDLINE("MOVE GF@$op1 int@1");
    ADDLINE("JUMPIFEQ $eqFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op1 int@0");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqOp1BoolOp2Float");
    ADDLINE("MOVE GF@$temp GF@$op1");
    ADDLINE("MOVE GF@$op1 float@0x1p+0");
    ADDLINE("JUMPIFEQ $eqFinish GF@$temp bool@true");
    ADDLINE("MOVE GF@$op1 float@0x0p+0");
    ADDLINE("JUMP $eqFinish");

    ADDLINE("LABEL $eqNotEqual");
    ADDLINE("PUSHS bool@false");
    ADDLINE("RETURN");

    ADDLINE("LABEL $eqFinish");
    ADDLINE("PUSHS GF@$op1");
    ADDLINE("PUSHS GF@$op2");
    ADDLINE("EQS");
    ADDLINE("RETURN");

    ADDLINE("LABEL $$main");

    str_append(&code, tempCode.str);
    str_copy(&tempCode, "");

    return NO_ERROR;
}

void generator_end() {

    stack_free(ifStack);
    stack_free(whileStack);

    str_destroy(&code);
    str_destroy(&line);
    str_destroy(&helper);
    str_destroy(&tempCode);

}

void gen_defvar(char *varName, bool global) {
    str_append(&code, "DEFVAR ");
    str_append(&code, ISGLOBAL(global));
    str_append(&code, varName);
    str_append(&code, "\n");
}

void gen_var(char *varName, bool global) {
    str_concat(&line, ISGLOBAL(global), varName, NULL);

    ADDCODE("DEFVAR "); ADDLINE(line.str);
    ADDCODE("POPS "); ADDLINE(line.str);
}

void gen_pushs(Token token, bool global) {
    if (token.type == TOKEN_STRING || token.type == TOKEN_IDENTIFIER) {
        if (token.type == TOKEN_STRING) {
            str_concat(&line, "PUSHS string@", replace_space(token.attribute.string.str), NULL);
        } else if (token.type == TOKEN_IDENTIFIER) {
            str_concat(&line, "PUSHS ", ISGLOBAL(global), token.attribute.string.str, NULL);
        }

        ADDLINE(line.str);
    } else {
        char temp[200];

        if (token.type == TOKEN_INTEGER) {
            sprintf(temp, "PUSHS int@%d", token.attribute.integer);
        } else if (token.type == TOKEN_DECIMAL) {
            sprintf(temp, "PUSHS float@%a", token.attribute.decimal);
        } else {
            strcpy(temp, "PUSHS nil@nil");
        }

        ADDLINE(temp);
    }
}

void gen_pops(char *varName, bool global) {
    ADDCODE("POPS "); ADDCODE(ISGLOBAL(global)); ADDLINE(varName);
}

void gen_stack(const char *instruct) {
    ADDLINE("POPS GF@$op2");
    ADDLINE("POPS GF@$op1");
    ADDLINE("TYPE GF@$op1Type GF@$op1");
    ADDLINE("TYPE GF@$op2Type GF@$op2");

    // TODO maybe relplace every str_concat with this: (str_concat is practically useless)
    ADDCODE("CALL $"); ADDLINE(instruct);
}

void gen_lts() {
    ADDLINE("CALL $glt");

    ADDLINE("LTS");
}

void gen_gts() {
    ADDLINE("CALL $glt");

    ADDLINE("GTS");
}

void gen_gtes() {
    ADDLINE("CALL $gtes");
}

void gen_ltes() {
    ADDLINE("CALL $ltes");
}

// TODO bool support is not required?
void gen_eqs() {
    gen_instruct("CALL $eq");
}

void gen_neqs() {
    gen_instruct("CALL $neq");
}

// generate instruction with no parameters
void gen_instruct(const char *instruct) {
    ADDLINE(instruct);
}

void gen_if_start() {
    stack_push(ifStack, ifUID);
    stack_push(ifStack, ifUID);
    stack_push(ifStack, ifUID);
    sprintf(uidStr, "%d", ifUID);
    ifUID++;

    ADDLINE("CALL $eval");

    ADDCODE("JUMPIFEQ $if"); ADDCODE(uidStr); ADDLINE("Else GF@$temp bool@false");
}

void gen_if_end() {
    int top = stack_pop(ifStack, &error).integer;
    sprintf(uidStr, "%d", top);

    ADDCODE("JUMP $if"); ADDCODE(uidStr); ADDLINE("End");
}

void gen_else_start() {
    int top = stack_pop(ifStack, &error).integer;
    sprintf(uidStr, "%d", top);

    ADDCODE("LABEL $if"); ADDCODE(uidStr); ADDLINE("Else");
}

void gen_else_end() {
    int top = stack_pop(ifStack, &error).integer;
    sprintf(uidStr, "%d", top);

    ADDCODE("LABEL $if"); ADDCODE(uidStr); ADDLINE("End");
}



void gen_while_start() {
    stack_push(whileStack, whileUID);
    stack_push(whileStack, whileUID);
    sprintf(uidStr, "%d", whileUID);
    whileUID++;
    
    ADDCODE("LABEL $while"); ADDCODE(uidStr); ADDLINE("Begin");
}

void gen_while_eval() {
    int top = stack_pop(whileStack, &error).integer;
    sprintf(uidStr, "%d", top);

    ADDLINE("CALL $eval");

    ADDCODE("JUMPIFEQ $while"); ADDCODE(uidStr); ADDLINE("End GF@$temp bool@false");
}

void gen_while_end() {
    int top = stack_pop(whileStack, &error).integer;
    sprintf(uidStr, "%d", top);

    ADDCODE("JUMP $while"); ADDCODE(uidStr); ADDLINE("Begin");

    ADDCODE("LABEL $while"); ADDCODE(uidStr); ADDLINE("End");
}

void gen_move_retval(char *varName, bool global) {
    ADDCODE("MOVE "); ADDCODE(ISGLOBAL(global)); ADDCODE(varName); ADDLINE(" TF@%retval");
}

void gen_func_def_start(char *funcName) {
    defParamCounter = 0;

    str_copy(&currFuncName, funcName);
    ADDCODE("JUMP %"); ADDCODE(funcName); ADDLINE("End");

    ADDCODE("LABEL %"); ADDLINE(funcName);
    ADDLINE("PUSHFRAME");

    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("MOVE LF@%retval nil@nil");

    str_append(&code, tempCode.str);
    str_copy(&tempCode, "");
}

void gen_func_def_add_param(char *paramName) {
    char temp[100];
    sprintf(temp, "%d", ++defParamCounter);

    ADDCODE("DEFVAR LF@"); ADDLINE(paramName);
    ADDCODE("MOVE LF@"); ADDCODE(paramName); ADDCODE(" LF@%"); ADDLINE(temp);
}

void gen_func_def_return() {
    ADDLINE("POPS LF@%retval");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");
}

void gen_func_def_end() {
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");
    ADDCODE("LABEL %"); ADDCODE(currFuncName.str); ADDLINE("End");

    str_append(&code, tempCode.str);
    str_copy(&tempCode, "");

    paramCounter = 0;
}

void gen_func_call_start() {
    ADDLINE("CREATEFRAME");
}

void gen_func_call_add_param(Token token, bool global) {
    char temp[200];
    sprintf(temp, "%d", ++paramCounter);

    ADDCODE("DEFVAR TF@%"); ADDLINE(temp);
    ADDCODE("MOVE TF@%"); ADDCODE(temp); ADDCODE(" ");

    if (token.type == TOKEN_STRING || token.type == TOKEN_IDENTIFIER) {
        if (token.type == TOKEN_STRING) {
            str_concat(&line, "string@", replace_space(token.attribute.string.str), NULL);
        } else if (token.type == TOKEN_IDENTIFIER) {
            str_concat(&line, ISGLOBAL(global), token.attribute.string.str, NULL);
        }

        ADDLINE(line.str);
    } else {
        if (token.type == TOKEN_INTEGER) {
            sprintf(temp, "int@%d", token.attribute.integer);
        } else if (token.type == TOKEN_DECIMAL) {
            sprintf(temp, "float@%a", token.attribute.decimal);
        } else if (token.type == KEYWORD_NONE) {
            strcpy(temp, "nil@nil");
        }

        ADDLINE(temp);
    }
}

void gen_func_call_end(char *funcName, char *varName, bool global) {
    ADDCODE("CALL %"); ADDLINE(funcName);
    
    if (varName) {
        ADDCODE("MOVE "); ADDCODE(ISGLOBAL(global)); ADDCODE(varName); ADDLINE(" TF@%retval");
    }

    paramCounter = 0;
}

void gen_func_builtin_call_end(char *funcName) {
    ADDCODE("CALL $"); ADDLINE(funcName);
    paramCounter = 0;
}

void gen_print(bool global, Token token) {
    if (token.type == TOKEN_IDENTIFIER) {
        ADDCODE("MOVE GF@$op1 "); ADDCODE(ISGLOBAL(global)); ADDLINE(token.attribute.string.str);
        ADDLINE("CALL $print");
    } else if (token.type == TOKEN_STRING) {
        ADDCODE("WRITE string@");
        ADDLINE(replace_space(token.attribute.string.str));
    } else {
        char temp[200];
        
        ADDCODE("WRITE string@");
        if (token.type == TOKEN_INTEGER) {
            sprintf(temp, "%d", token.attribute.integer);
        } else if (token.type ==  TOKEN_DECIMAL) {
            sprintf(temp, "%a", token.attribute.decimal);
        } else if (token.type == KEYWORD_NONE) {
            strcpy(temp, "None");
        }

        ADDLINE(temp);
    }
}

void gen_print_space() {
    ADDLINE("WRITE string@\\032");
}

void gen_print_end() {
    ADDLINE("WRITE string@\\010");

    // return value is always None
    ADDLINE("CREATEFRAME");
    ADDLINE("DEFVAR TF@%retval");
    ADDLINE("MOVE TF@%retval nil@nil");
}

// TODO maybe change name because it now does more than just replace spaces
const char *replace_space (char *string) {
    char temp[100];
    str_copy(&helper, "");

    for (size_t j = 0; j < strlen(string); j++) {
        if (isspace(string[j]) || string[j] == '#' || string[j] == '\\' || string[j] == '\"' || string[j] == '\'' || string[j] <= 32 || string[j] == 35) {
            sprintf(temp, "\\%03d", string[j]);
        } else {
            sprintf(temp, "%c", string[j]);
        }
        
        str_append(&helper, temp);
    }

    return helper.str;
}

void store_top() {
    ADDLINE("POPS GF@$temp");
    ADDLINE("PUSHS GF@$temp");
}

void push_top() {
    ADDLINE("PUSHS GF@$temp");
}

void generate_code(FILE *destFile) {
    str_append(&code, tempCode.str);

    fputs(code.str, destFile);
    generator_end();
}