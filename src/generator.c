#include "generator.h"

#define ADDLINE(line) \
do { \
str_append(&code, line); \
str_append(&code, "\n"); \
 } while (0) \

#define ADDCODE(line) \
str_append(&code, line); \

#define ISGLOBAL(global) \
global ? "GF@" : "LF@" \

#define STRLEN 128

tString code;
tString line;
tString helper;

int error;
int uid = 0;
char uidStr[STRLEN];

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

    ADDLINE("JUMPIFEQ $idivsSameType GF@$op1Type GF@$op2Type");
    ADDLINE("JUMPIFEQ $idivsOp1Int GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $idivsOp1Float GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $idivsOp1Int");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDLINE("FLOAT2INT GF@$op2 GF@$op2");
    ADDLINE("JUMP $idivsFinish");

    ADDLINE("LABEL $idivsOp1Float");
    ADDLINE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDLINE("FLOAT2INT GF@$op1 GF@$op1");
    ADDLINE("JUMP $idivsFinish");

    ADDLINE("LABEL $idivsSameType");
    ADDLINE("JUMPIFEQ $idivsFinish GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $idivsSameTypeFloat GF@$op1Type string@float");
    ADDLINE("JUMP $error");

    ADDLINE("LABEL $idivsSameTypeFloat");
    ADDLINE("FLOAT2INT GF@$op1 GF@$op1");
    ADDLINE("FLOAT2INT GF@$op2 GF@$op2");
    ADDLINE("JUMP $idivsFinish");

    ADDLINE("LABEL $idivsFinish");
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
    ADDLINE("DEFVAR LF@%retval");
    ADDLINE("DEFVAR LF@letter");
    ADDLINE("DEFVAR LF@cond");
    ADDLINE("DEFVAR LF@limit");
    ADDLINE("DEFVAR LF@len");

    ADDLINE("MOVE LF@%retval nil@nil");

    ADDLINE("TYPE GF@$temp LF@%1");
    ADDLINE("JUMPIFNEQ $substrFinish GF@$temp string@string");

    ADDLINE("TYPE GF@$temp LF@%2");
    ADDLINE("JUMPIFNEQ $substrFinish GF@$temp string@int");

    ADDLINE("TYPE GF@$temp LF@%3");
    ADDLINE("JUMPIFNEQ $substrFinish GF@$temp string@int");

    ADDLINE("STRLEN LF@len LF@%1");
    ADDLINE("GT LF@cond LF@%2 LF@len");
    ADDLINE("JUMPIFEQ $ordNone LF@cond bool@true");

    ADDLINE("LT LF@cond LF@%2 int@0");
    ADDLINE("JUMPIFEQ $ordNone LF@cond bool@true");

    ADDLINE("MOVE LF@%retval string@");
    ADDLINE("ADD LF@limit LF@%2 LF@%3");
    
    ADDLINE("EQ LF@cond LF@%3 int@0");
    ADDLINE("JUMPIFEQ $substrFinish LF@cond bool@true");

    ADDLINE("LABEL $substrLoop");
    ADDLINE("GETCHAR LF@letter LF@%1 LF@%2");
    ADDLINE("CONCAT LF@%retval LF@%retval LF@letter");
    ADDLINE("ADD LF@%2 LF@%2 int@1");

    ADDLINE("GT LF@cond LF@len LF@%2");
    ADDLINE("JUMPIFEQ $substrFinish LF@cond bool@false");

    ADDLINE("GT LF@cond LF@limit LF@%2");
    ADDLINE("JUMPIFEQ $substrLoop LF@cond bool@true");

    ADDLINE("LABEL $substrFinish");
    ADDLINE("POPFRAME");
    ADDLINE("RETURN");

    // function len
    ADDLINE("LABEL $len");
    ADDLINE("PUSHFRAME");
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
    ADDLINE("JUMPIFNEQ $ordNone LF@type string@string");

    ADDLINE("TYPE LF@type LF@%2");
    ADDLINE("JUMPIFNEQ $ordNone LF@type string@int");

    ADDLINE("STRLEN LF@len LF@%1");
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

    return NO_ERROR;
}

void generator_end() {

    ADDLINE("popframe");
    ADDLINE("clears");

    str_destroy(&code);
    str_destroy(&line);
    str_destroy(&helper);

}

void gen_var(char *varName, bool global) {
    str_concat(&line, ISGLOBAL(global), varName, NULL);

    ADDCODE("DEFVAR "); ADDLINE(line.str);
    ADDCODE("POPS "); ADDLINE(line.str);
}

// add, sub, mul, lt, or, ... every function with 2 symb
// void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool global) {

//     char scope[STRLEN];
//     setScope(scope, global);

//     if (type == TYPE_FLOAT) {
//         char temp[STRLEN];
//         sprintf(temp, " float@%a float@%a", symb1.decimal, symb2.decimal);

//         str_concat(&line, instruct, scope, var, temp, NULL);
//     } else {
//         char varType[STRLEN];
//         if (type == TYPE_INT) {
//             strcpy(varType, " int@");
//         } else if (type == TYPE_STRING) {
//             strcpy(varType, " string@");
//         } else if (type == TYPE_BOOL) {
//             strcpy(varType, " bool@");
//         }
        
//         str_concat(&line, instruct, scope, var, varType, symb1.string, varType, symb2.string, NULL);
//     }

//     ADDLINE(line.str);
// }

// void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool global) {

//     char scope[STRLEN];
//     setScope(scope, global);

//     if (type == TYPE_FLOAT) {
//         char temp[STRLEN];
//         sprintf(temp, " float@%a", symb.decimal);

//         str_concat(&line, instruct, scope, var, temp, NULL);
//     } else {
//         char varType[STRLEN];
//         if (type == TYPE_INT) {
//             strcpy(varType, " int@");
//         } else if (type == TYPE_STRING) {
//             strcpy(varType, " string@");
//         } else if (type == TYPE_BOOL) {
//             strcpy(varType, " bool@");
//         }
        
//         str_concat(&line, instruct, scope, var, varType, symb.string, NULL);
//     }

//     ADDLINE(line.str);
// }

void gen_pushs(Token token, bool global) {

    char temp[STRLEN];

    if (token.type == TOKEN_INTEGER) {
        sprintf(temp, "PUSHS int@%d", token.attribute.integer);
    } else if (token.type == TOKEN_DECIMAL) {
        sprintf(temp, "PUSHS float@%a", token.attribute.decimal);
    } else if (token.type == TOKEN_STRING) {
        sprintf(temp, "PUSHS string@%s", replace_space(token.attribute.string.str));
    } else if (token.type == TOKEN_IDENTIFIER) {
        str_concat(&line, "PUSHS ", ISGLOBAL(global), token.attribute.string.str, NULL);
        strcpy(temp, line.str);
    }

    ADDLINE(temp);
    
}

void gen_pops(char *var, bool global) {
    str_concat(&line, "POPS ", ISGLOBAL(global), var, NULL);

    ADDLINE(line.str);

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

// TODO bool support is not required?
void gen_eqs() {
    gen_instruct("CALL $eq");
}

// generate instruction with no parameter
void gen_instruct(const char *instruct) {
    ADDLINE(instruct);
}

void gen_if_start() {
    sprintf(uidStr, "%d", uid++);

    ADDLINE("CALL $eval");

    ADDCODE("JUMPIFEQ $if"); ADDCODE(uidStr); ADDLINE("Else GF@$temp bool@false");
}

void gen_if_end() {
    ADDCODE("JUMP $if"); ADDCODE(uidStr); ADDLINE("End");
}

void gen_else_start() {
    ADDCODE("LABEL $if"); ADDCODE(uidStr); ADDLINE("Else");
}

void gen_else_end() {
    ADDCODE("JUMP $if"); ADDCODE(uidStr); ADDLINE("End");

    ADDCODE("LABEL $if"); ADDCODE(uidStr); ADDLINE("End");
}

// TODO while requires you to push the evaluated value onto the stack in body every time (even when the value doesn't change)
void gen_while_start() {
    sprintf(uidStr, "%d", uid++);

    ADDCODE("LABEL $while"); ADDCODE(uidStr); ADDLINE("Begin");

    ADDLINE("CALL $eval");

    ADDCODE("JUMPIFEQ $while"); ADDCODE(uidStr); ADDLINE("End GF@$temp bool@false");
}

void gen_while_end() {
    ADDCODE("JUMP $while"); ADDCODE(uidStr); ADDLINE("Begin");

    ADDCODE("LABEL $while"); ADDCODE(uidStr); ADDLINE("End");
}

// void gen_inputs() {
    
// }

// TODO print must return None
void gen_print(unsigned n, bool global, Token token, ...) {

    va_list ap;
    char temp[STRLEN];

    va_start(ap, token);

    for (unsigned i = 0; i < n; i++) {
        if (token.type == TOKEN_IDENTIFIER) {

        } else {
            ADDCODE("WRITE string@");

            if (token.type == TOKEN_INTEGER) {
                sprintf(temp, "%d", token.attribute.integer);
            } else if (token.type ==  TOKEN_DECIMAL) {
                sprintf(temp, "%a", token.attribute.decimal);
            } else if (token.type == KEYWORD_NONE) {
                strcpy(temp, "None");
            } else if (token.type == TOKEN_STRING) {
                strcpy(temp, replace_space(token.attribute.string.str));
            }

            ADDCODE(temp);
            if (i == n - 1) {
                // last term, add new line
                ADDLINE("\\010");
            } else {
                // add space
                ADDLINE("\\032");
            }
        }

        token = va_arg(ap, Token);
    }

    va_end(ap);

    // return value is always None
    ADDLINE("CREATEFRAME");
    ADDLINE("DEFVAR TF@%retval");
    ADDLINE("MOVE TF@%retval nil@nil");

}

// TODO we must convert every ASCII chars from 000-032 035 and 092, not just spaces !
const char *replace_space (char *string) {

    str_copy(&helper, "");

    for (size_t j = 0; j < strlen(string); j++) {
        if (string[j] == ' ') {
            str_append(&helper, "\\032");
        } else {
            char temp[2];
            temp[0] = string[j];
            temp[1] = '\0';
            str_append(&helper, temp);
        }
    }

    return helper.str;
}

// TODO remove?
void gen_defvar(char *var) {
    str_concat(&line, "defvar GF@", var, NULL);

    ADDLINE(line.str);
}

void generate_code(FILE *destFile) {
    fputs(code.str, destFile);
    generator_end();
}