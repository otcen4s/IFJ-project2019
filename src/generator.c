#include "generator.h"

#define ADDCODE(line) \
do { \
str_append(&code, line); \
str_append(&code, "\n"); \
 } while (0) \

tString code;
tString line;
tString helper;

int error;

#define STRLEN 128

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
    ADDCODE(".IFJcode19");

    // TODO defining global variables to use in functions, maybe not the best approach
    ADDCODE("DEFVAR GF@$op1");
    ADDCODE("DEFVAR GF@$op2");
    ADDCODE("DEFVAR GF@$op1Type");
    ADDCODE("DEFVAR GF@$op2Type");
    ADDCODE("DEFVAR GF@$temp");
    
    ADDCODE("JUMP $$main");
    
    ADDCODE("LABEL $error");
    ADDCODE("EXIT int@4");

    ADDCODE("LABEL $divisionByZero");
    ADDCODE("EXIT int@9");
    
    // defining function adds
    ADDCODE("LABEL $adds");

    ADDCODE("JUMPIFEQ $addsSameType GF@$op1Type GF@$op2Type");
    ADDCODE("JUMPIFEQ $addsOp1Int GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $addsOp1Float GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $addsOp1Int");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $addsAddFinish");

    ADDCODE("LABEL $addsOp1Float");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $addsAddFinish");

    ADDCODE("LABEL $addsSameType");
    ADDCODE("JUMPIFEQ $addsAddFinish GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $addsAddFinish GF@$op1Type string@float");
    ADDCODE("JUMPIFEQ $addsConcatFinish GF@$op1Type string@string");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $addsAddFinish");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("ADDS");
    ADDCODE("RETURN");

    ADDCODE("LABEL $addsConcatFinish");
    ADDCODE("CONCAT GF@$temp GF@$op1 GF@$op2");
    ADDCODE("PUSHS GF@$temp");
    ADDCODE("RETURN");
    
    // defining function subs
    ADDCODE("LABEL $subs");

    ADDCODE("JUMPIFEQ $subsSameType GF@$op1Type GF@$op2Type");
    ADDCODE("JUMPIFEQ $subsOp1Int GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $subsOp1Float GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $subsOp1Int");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $subsFinish");

    ADDCODE("LABEL $subsOp1Float");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $subsFinish");

    ADDCODE("LABEL $subsSameType");
    ADDCODE("JUMPIFEQ $subsFinish GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $subsFinish GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $subsFinish");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("SUBS");
    ADDCODE("RETURN");

    // defining function muls
    ADDCODE("LABEL $muls");

    ADDCODE("JUMPIFEQ $mulsSameType GF@$op1Type GF@$op2Type");
    ADDCODE("JUMPIFEQ $mulsOp1Int GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $mulsOp1Float GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $mulsOp1Int");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $mulsFinish");

    ADDCODE("LABEL $mulsOp1Float");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $mulsFinish");

    ADDCODE("LABEL $mulsSameType");
    ADDCODE("JUMPIFEQ $mulsFinish GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $mulsFinish GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $mulsFinish");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("MULS");
    ADDCODE("RETURN");

    // defining function divs
    ADDCODE("LABEL $divs");

    ADDCODE("JUMPIFEQ $divsSameType GF@$op1Type GF@$op2Type");
    ADDCODE("JUMPIFEQ $divsOp1Int GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $divsOp1Float GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $divsOp1Int");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $divsFinish");

    ADDCODE("LABEL $divsOp1Float");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $divsFinish");

    ADDCODE("LABEL $divsSameType");
    ADDCODE("JUMPIFEQ $divsFinish GF@$op1Type string@float");
    ADDCODE("JUMPIFEQ $divsSameTypeInt GF@$op1Type string@int");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $divsSameTypeInt");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $divsFinish");

    ADDCODE("LABEL $divsFinish");
    ADDCODE("JUMPIFEQ $divisionByZero GF@$op2 float@0x0p+0");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("DIVS");
    ADDCODE("RETURN");

    // defining function idivs
    ADDCODE("LABEL $idivs");

    ADDCODE("JUMPIFEQ $idivsSameType GF@$op1Type GF@$op2Type");
    ADDCODE("JUMPIFEQ $idivsOp1Int GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $idivsOp1Float GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $idivsOp1Int");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@float");
    ADDCODE("FLOAT2INT GF@$op2 GF@$op2");
    ADDCODE("JUMP $idivsFinish");

    ADDCODE("LABEL $idivsOp1Float");
    ADDCODE("JUMPIFNEQ $error GF@$op2Type string@int");
    ADDCODE("FLOAT2INT GF@$op1 GF@$op1");
    ADDCODE("JUMP $idivsFinish");

    ADDCODE("LABEL $idivsSameType");
    ADDCODE("JUMPIFEQ $idivsFinish GF@$op1Type string@int");
    ADDCODE("JUMPIFEQ $idivsSameTypeFloat GF@$op1Type string@float");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $idivsSameTypeFloat");
    ADDCODE("FLOAT2INT GF@$op1 GF@$op1");
    ADDCODE("FLOAT2INT GF@$op2 GF@$op2");
    ADDCODE("JUMP $idivsFinish");

    ADDCODE("LABEL $idivsFinish");
    ADDCODE("JUMPIFEQ $divisionByZero GF@$op2 int@0");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("IDIVS");
    ADDCODE("RETURN");

    ADDCODE("LABEL $$main");

    return NO_ERROR;
}

void generator_end() {

    ADDCODE("popframe");
    ADDCODE("clears");

    str_destroy(&code);
    str_destroy(&line);
    str_destroy(&helper);

}

// void gen_var(char *var, Type type, Value value, bool isGlobal) {
//     char scope[STRLEN];
//     setScope(scope, isGlobal);

//     str_concat(&line, "defvar", scope, var, NULL);
//     ADDCODE(line.str);

//     if (type == TYPE_STRING) {
//         str_concat(&line, "move", scope, var, " string@", value.string, NULL);
//     } else if (type == TYPE_INT) {
//         str_concat(&line, "move", scope, var, " int@", value.string, NULL);
//     } else if (type == TYPE_FLOAT) {
//         char temp[STRLEN];
//         sprintf(temp, "%a", value.decimal);
//         str_concat(&line, "move", scope, var, " float@", temp, NULL);
//     } else {
//         str_concat(&line, "move", scope, var, " nil@nil", NULL);
//     }

//     ADDCODE(line.str);
// }

void setScope(char *scope, bool isGlobal) {
    if (isGlobal) {
        strcpy(scope, " GF@");
    } else {
        strcpy(scope, " LF@");
    }
}

// add, sub, mul, lt, or, ... every function with 2 symb
// void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool isGlobal) {

//     char scope[STRLEN];
//     setScope(scope, isGlobal);

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

//     ADDCODE(line.str);
// }

// void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool isGlobal) {

//     char scope[STRLEN];
//     setScope(scope, isGlobal);

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

//     ADDCODE(line.str);
// }

void gen_pushs(Token token, bool isGlobal) {

    char temp[STRLEN];

    if (token.type == TOKEN_INTEGER) {
        sprintf(temp, "PUSHS int@%d", token.attribute.integer);
    } else if (token.type == TOKEN_DECIMAL) {
        sprintf(temp, "PUSHS float@%a", token.attribute.decimal);
    } else if (token.type == TOKEN_STRING) {
        sprintf(temp, "PUSHS string@%s", replace_space(token.attribute.string.str));
    } else if (token.type == TOKEN_IDENTIFIER) {
        str_concat(&line, "PUSHS ", (isGlobal) ? "GF@" : "LF@", token.attribute.string.str, NULL);
        strcpy(temp, line.str);
    }

    ADDCODE(temp);
    
}

void gen_pops(char *var, bool isGlobal) {
    str_concat(&line, "POPS ", (isGlobal) ? "GF@" : "LF@", var, NULL);

    ADDCODE(line.str);

}

void gen_stack(const char *instruct) {
    ADDCODE("POPS GF@$op2");
    ADDCODE("POPS GF@$op1");
    ADDCODE("TYPE GF@$op1Type GF@$op1");
    ADDCODE("TYPE GF@$op2Type GF@$op2");

    str_concat(&line, "CALL ", "$", instruct, NULL);

    ADDCODE(line.str);
}

void gen_print(unsigned n, bool isGlobal, Token token, ...) {

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
            str_concat(&line, "WRITE ", (isGlobal) ? "GF@" : "LF@", token.attribute.string.str, NULL);
        }

        ADDCODE(line.str);
        token = va_arg(ap, Token);
    }

    va_end(ap);

    // add new line at the end
    ADDCODE("WRITE string@\\010");

}

const char * replace_space(char *string) {

    str_copy(&helper, "");

    for (int j = 0; j < strlen(string); j++) {
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

    ADDCODE(line.str);
}

void generate_code(FILE *destFile) {
    fputs(code.str, destFile);
    generator_end();
}