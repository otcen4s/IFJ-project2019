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

    // if function definition
    ADDLINE("LABEL $if");
    ADDLINE("POPS GF@$op1");
    ADDLINE("TYPE GF@$op1Type GF@$op1");

    ADDLINE("JUMPIFEQ $ifInt GF@$op1Type string@int");
    ADDLINE("JUMPIFEQ $ifFloat GF@$op1Type string@float");
    ADDLINE("JUMPIFEQ $ifString GF@$op1Type string@string");
    ADDLINE("JUMPIFEQ $ifFalse GF@$op1Type string@nil");
    ADDLINE("JUMPIFEQ $ifBool GF@$op1Type string@bool");

    ADDLINE("LABEL $ifInt");
    ADDLINE("JUMPIFEQ $ifFalse GF@$op1 int@0");
    ADDLINE("JUMP $ifTrue");

    ADDLINE("LABEL $ifFloat");
    ADDLINE("JUMPIFEQ $ifFalse GF@$op1 float@0x0p+0");
    ADDLINE("JUMP $ifTrue");

    ADDLINE("LABEL $ifString");
    ADDLINE("JUMPIFEQ $ifFalse GF@$op1 string@");
    ADDLINE("JUMP $ifTrue");

    ADDLINE("LABEL $ifBool");
    ADDLINE("JUMPIFEQ $ifFalse GF@$op1 bool@false");
    ADDLINE("JUMP $ifTrue");

    ADDLINE("LABEL $ifTrue");
    ADDLINE("MOVE GF@$temp bool@true");
    ADDLINE("RETURN");

    ADDLINE("LABEL $ifFalse");
    ADDLINE("MOVE GF@$temp bool@false");
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

// generate instruction with no parameter
void gen_instruct(const char *instruct) {
    ADDLINE(instruct);
}

void gen_if_start() {
    sprintf(uidStr, "%d", uid++);

    ADDLINE("CALL $if");

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

// TODO print must return None
void gen_print(unsigned n, bool global, Token token, ...) {

    va_list ap;
    char temp[STRLEN];

    va_start(ap, token);

    for (unsigned i = 0; i < n; i++) {
        str_copy(&line, "WRITE string@");

        if (token.type == TOKEN_INTEGER) {
            sprintf(temp, "%d", token.attribute.integer);
            str_append(&line, temp);
        } else if (token.type ==  TOKEN_DECIMAL) {
            sprintf(temp, "%a", token.attribute.decimal);
            str_append(&line, temp);
        } else if (token.type == KEYWORD_NONE) {
            str_append(&line, "None");
        } else if (token.type == TOKEN_STRING) {
            str_append(&line, replace_space(token.attribute.string.str));
        } else if (token.type == TOKEN_IDENTIFIER) {
            str_concat(&line, "WRITE ", ISGLOBAL(global), token.attribute.string.str, NULL);
        }

        ADDLINE(line.str);
        token = va_arg(ap, Token);
    }

    va_end(ap);

    // add new line at the end
    ADDLINE("WRITE string@\\010");

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