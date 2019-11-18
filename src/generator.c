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
    // ADDCODE("label $main");
    // ADDCODE("createframe");
    // ADDCODE("pushframe");

    return NO_ERROR;
}

void generator_end() {

    ADDCODE("popframe");
    ADDCODE("clears");

    str_destroy(&code);
    str_destroy(&line);

}

void gen_var(char *var, Type type, Value value, bool isGlobal) {
    char scope[STRLEN];
    setScope(scope, isGlobal);

    str_concat(&line, "defvar", scope, var, NULL);
    ADDCODE(line.str);

    if (type == TYPE_STRING) {
        str_concat(&line, "move", scope, var, " string@", value.string, NULL);
    } else if (type == TYPE_INT) {
        str_concat(&line, "move", scope, var, " int@", value.string, NULL);
    } else if (type == TYPE_FLOAT) {
        char temp[STRLEN];
        sprintf(temp, "%a", value.decimal);
        str_concat(&line, "move", scope, var, " float@", temp, NULL);
    } else {
        str_concat(&line, "move", scope, var, " nil@nil", NULL);
    }

    ADDCODE(line.str);
}

void setScope(char *scope, bool isGlobal) {
    if (isGlobal) {
        strcpy(scope, " GF@");
    } else {
        strcpy(scope, " LF@");
    }
}

// add, sub, mul, lt, or, ... every function with 2 symb
void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool isGlobal) {

    char scope[STRLEN];
    setScope(scope, isGlobal);

    if (type == TYPE_FLOAT) {
        char temp[STRLEN];
        sprintf(temp, " float@%a float@%a", symb1.decimal, symb2.decimal);

        str_concat(&line, instruct, scope, var, temp, NULL);
    } else {
        char varType[STRLEN];
        if (type == TYPE_INT) {
            strcpy(varType, " int@");
        } else if (type == TYPE_STRING) {
            strcpy(varType, " string@");
        } else if (type == TYPE_BOOL) {
            strcpy(varType, " bool@");
        }
        
        str_concat(&line, instruct, scope, var, varType, symb1.string, varType, symb2.string, NULL);
    }

    ADDCODE(line.str);
}

void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool isGlobal) {

    char scope[STRLEN];
    setScope(scope, isGlobal);

    if (type == TYPE_FLOAT) {
        char temp[STRLEN];
        sprintf(temp, " float@%a", symb.decimal);

        str_concat(&line, instruct, scope, var, temp, NULL);
    } else {
        char varType[STRLEN];
        if (type == TYPE_INT) {
            strcpy(varType, " int@");
        } else if (type == TYPE_STRING) {
            strcpy(varType, " string@");
        } else if (type == TYPE_BOOL) {
            strcpy(varType, " bool@");
        }
        
        str_concat(&line, instruct, scope, var, varType, symb.string, NULL);
    }

    ADDCODE(line.str);
}

// gen_pushs(symb, Type type, Symb symb) {

//     ADDCODE(strcat("PUSHS ", ));

// }

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