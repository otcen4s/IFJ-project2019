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

    ADDCODE("JUMP $$main");

    ADDCODE("LABEL $adds");

    ADDCODE("POPS GF@$op1");
    ADDCODE("TYPE GF@$opType1 GF@$op1");

    ADDCODE("POPS GF@$op2");
    ADDCODE("TYPE GF@$opType2 GF@$op2");

    ADDCODE("JUMPIFEQ $isInt GF@$opType1 string@int");
    ADDCODE("JUMPIFEQ $isFloat GF@$opType1 string@float");
    ADDCODE("JUMPIFEQ $isString GF@$opType1 string@string");
    ADDCODE("EXIT int@4");

    ADDCODE("LABEL $isIntFloat");
    ADDCODE("INT2FLOAT GF@$op1 GF@$op1");
    ADDCODE("JUMP $genAdd");

    ADDCODE("LABEL $isFloatInt");
    ADDCODE("INT2FLOAT GF@$op2 GF@$op2");
    ADDCODE("JUMP $genAdd");

    ADDCODE("LABEL $isInt");
    ADDCODE("JUMPIFEQ $genAdd GF@$opType1 GF@$opType2");
    ADDCODE("JUMPIFEQ $isIntFloat GF@$opType2 string@float");
    ADDCODE("EXIT int@4");

    ADDCODE("LABEL $isFloat");
    ADDCODE("JUMPIFEQ $genAdd GF@$opType1 GF@$opType2");
    ADDCODE("JUMPIFEQ $isFloatInt GF@$opType2 string@int");
    ADDCODE("EXIT int@4");

    ADDCODE("LABEL $isString");
    ADDCODE("JUMPIFEQ $genConcat GF@$opType1 GF@$opType2");
    ADDCODE("EXIT int@4");

    ADDCODE("LABEL $genAdd");
    ADDCODE("PUSHS GF@$op1");
    ADDCODE("PUSHS GF@$op2");
    ADDCODE("ADDS");
    ADDCODE("RETURN");

    ADDCODE("LABEL $genConcat");
    ADDCODE("CONCAT GF@$temp GF@$op2 GF@$op1");
    ADDCODE("PUSHS GF@$temp");
    ADDCODE("RETURN");

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

void gen_pushs(Token token) {

    char temp[STRLEN];

    if (token.type == TOKEN_INTEGER) {
        sprintf(temp, "PUSHS int@%d", token.attribute.integer);
    } else if (token.type == TOKEN_DECIMAL) {
        sprintf(temp, "PUSHS float@%a", token.attribute.decimal);
    } else {
        sprintf(temp, "PUSHS string@%s", token.attribute.string.str);
    }

    ADDCODE(temp);
    
}

void gen_pops(char *var) {
    str_concat(&line, "POPS ", var, NULL);

    ADDCODE(line.str);

}

void gen_adds() {
    ADDCODE("CALL $adds");
}

void gen_print(char *printStr) {
    str_concat(&line, "write GF@", printStr, NULL);

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