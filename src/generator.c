#include "generator.h"

#define ADDCODE(line) \
do { \
str_append(&code, line); \
str_append(&code, "\n"); \
 } while (0) \

tString code;
tString line;

int error;

#define STRLEN 128

int generator_begin() {
    if ((error = str_init(&code))) {
        return error;
    }

    if ((error = str_init(&line))) {
        return error;
    }

    // generate header
    ADDCODE(".IFJcode19");

    // TODO defining global variables to use in functions, maybe not the best approach
    ADDCODE("DEFVAR GF@$op1");
    ADDCODE("DEFVAR GF@$op2");
    ADDCODE("DEFVAR GF@$opType1");
    ADDCODE("DEFVAR GF@$opType2");
    ADDCODE("DEFVAR GF@$temp");
    ADDCODE("DEFVAR GF@$operation");
    
    ADDCODE("JUMP $$main");
    
    ADDCODE("LABEL $error");
    ADDCODE("EXIT int@4");
    
    ADDCODE("LABEL $finish");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("RETURN");
    
    ADDCODE("LABEL $op1String");
    ADDCODE("JUMPIFNEQ $error GF@$opType2 string@string");
    ADDCODE("CONCAT GF@$temp GF@$op1 GF@$op2");
    ADDCODE("PUSHS GF@$temp");
    ADDCODE("RETURN");
    
    ADDCODE("LABEL $op1IntOp2Float");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $finish");
    
    ADDCODE("LABEL $op1Int");
    ADDCODE("JUMPIFEQ $error GF@$operation string@f");
    ADDCODE("JUMPIFEQ $finish GF@$opType2 string@int");
    ADDCODE("JUMPIFEQ $error GF@$operation string@i");
    ADDCODE("JUMPIFEQ $op1IntOp2Float GF@$opType2 string@float");
    ADDCODE("JUMP $error");
    
    ADDCODE("LABEL $op1FloatOp2Int");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $finish");
    
    ADDCODE("LABEL $op1Float");
    ADDCODE("JUMPIFEQ $finish GF@$opType2 string@float");
    ADDCODE("JUMPIFEQ $error GF@$operation string@i");
    ADDCODE("JUMPIFEQ $error GF@$operation string@f");
    ADDCODE("JUMPIFEQ $op1FloatOp2Int GF@$opType2 string@int");
    ADDCODE("JUMP $error");

    ADDCODE("LABEL $typeCheck");
    ADDCODE("POPS GF@$op1");
    ADDCODE("TYPE GF@$opType1 GF@$op1");
    
    ADDCODE("POPS GF@$op2");
    ADDCODE("TYPE GF@$opType2 GF@$op2");
    
    ADDCODE("JUMPIFEQ $op1Int GF@$opType1 string@int");
    ADDCODE("JUMPIFEQ $error GF@$operation string@i");
    
    ADDCODE("JUMPIFEQ $op1Float GF@$opType1 string@float");
    ADDCODE("JUMPIFEQ $error GF@$operation string@f");
    
    ADDCODE("JUMPIFNEQ $error GF@$operation string@ifs");
    ADDCODE("JUMPIFEQ $op1String GF@$opType1 string@string");
    ADDCODE("JUMP $error");
    
    ADDCODE("LABEL $$main");

    return NO_ERROR;
}

void generator_end() {

    ADDCODE("popframe");
    ADDCODE("clears");

    str_destroy(&code);
    str_destroy(&line);

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
        sprintf(temp, "PUSHS string@%s", token.attribute.string.str);
    } else if (token.type == TOKEN_IDENTIFIER) {
        str_concat(&line, "PUSHS ", (isGlobal) ? "GF@" : "LF@", token.attribute.string.str, NULL);
        strcpy(temp, line.str);
    }

    ADDCODE(temp);
    
}

void gen_pops(char *var) {
    str_concat(&line, "POPS ", var, NULL);

    ADDCODE(line.str);

}

void gen_adds() {
    ADDCODE("MOVE GF@$operation string@ifs");
    ADDCODE("CALL $typeCheck");

    ADDCODE("JUMPIFEQ $isString GF@$opType1 string@string");
    ADDCODE("ADDS");
    ADDCODE("LABEL $isString");

}

void gen_subs() {
    ADDCODE("MOVE GF@$operation string@if");
    ADDCODE("CALL $typeCheck");

    ADDCODE("SUBS");
}

void gen_muls() {
    ADDCODE("MOVE GF@$operation string@if");
    ADDCODE("CALL $typeCheck");

    ADDCODE("MULS");
}

void gen_divs() {
    ADDCODE("MOVE GF@$operation string@f");
    ADDCODE("CALL $typeCheck");

    ADDCODE("DIVS");
}

void gen_idivs() {
    ADDCODE("MOVE GF@$operation string@i");
    ADDCODE("CALL $typeCheck");

    ADDCODE("IDIVS");
}

void gen_print(unsigned n, Token token, ...) {

    va_list ap;
    char temp[STRLEN];

    va_start(ap, token);

    str_copy(&line, "WRITE string@");

    for (unsigned i = 0; i < n; i++) {
        if (token.type == TOKEN_INTEGER) {
            sprintf(temp, "%d", token.attribute.integer);
        } else if (token.type ==  TOKEN_DECIMAL) {
            sprintf(temp, "%a", token.attribute.decimal);
        } else if (token.type == KEYWORD_NONE) {
            strcpy(temp, "None");
        } else {
            // KEYWORD_STRING
            strcpy(temp, "");
            int outJ = 0;
            for (int j = 0; j < strlen(token.attribute.string.str); j++) {
                if (token.attribute.string.str[j] == ' ') {
                    temp[outJ++] = '\\';
                    temp[outJ++] = '0';
                    temp[outJ++] = '3';
                    temp[outJ++] = '2';
                } else {
                    temp[outJ++] = token.attribute.string.str[j];
                }
            }
        }

        str_append(&line, temp);
        token = va_arg(ap, Token);
    }

    va_end(ap);

    // add new line at the end
    str_append(&line, "\\010");

    ADDCODE(line.str);

}

void gen_defvar(char *var) {
    str_concat(&line, "defvar GF@", var, NULL);

    ADDCODE(line.str);
}

void generate_code(FILE *destFile) {
    fputs(code.str, destFile);
    generator_end();
}