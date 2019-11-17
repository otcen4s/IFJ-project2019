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
    ADDCODE("label $main");
    ADDCODE("createframe");
    ADDCODE("pushframe");

    return NO_ERROR;
}

void generator_end() {

    ADDCODE("popframe");
    ADDCODE("clears");

    str_destroy(&code);
    str_destroy(&line);
    str_destroy(&helper);

}

void gen_var(char *var, Type type, Value value, bool isGlobal) {
    setScope(isGlobal);
    char *scope = helper.str;

    str_concat(&line, "defvar", scope, var, NULL);
    ADDCODE(line.str);

    if (type == TYPE_STRING) {
        str_concat(&line, "move", scope, var, " string@", value.string, NULL);
    } else if (type == TYPE_INT) {
        str_concat(&line, "move", scope, var, " int@", value.string, NULL);
    } else if (type == TYPE_FLOAT) {
        char temp[128];
        sprintf(temp, "%a", value.decimal);
        str_concat(&line, "move", scope, var, " float@", temp, NULL);
    } else {
        str_concat(&line, "move", scope, var, " nil@nil", NULL);
    }

    ADDCODE(line.str);
}

void setScope(bool isGlobal) {
    if (isGlobal) {
        str_copy(&helper, " GF@");
    } else {
        str_copy(&helper, " LF@");
    }
}

// add, sub, mul, div, idiv
void gen_arit(char *instruct, Type type, char *var, Value symb1, Value symb2, bool isGlobal) {

    // TODO not the best approach, bad readability
    setScope(isGlobal);
    char *scope = helper.str;

    if (type == TYPE_FLOAT) {
        char temp[128];
        sprintf(temp, " float@%a float@%a", symb1.decimal, symb2.decimal);

        str_concat(&line, instruct, scope, var, temp, NULL);
    } else {
        if (type == TYPE_INT) {
            str_concat(&line, instruct, scope, var, " int@", symb1.string, " int@", symb2.string, NULL);
        } else if (type == TYPE_STRING) {
            str_concat(&line, instruct, scope, var, " string@", symb1.string, " string@", symb2.string, NULL);
        }
    }

    ADDCODE(line.str);
}

void gen_print(char *printStr) {
    str_concat(&line, "write gf@", printStr, NULL);

    ADDCODE(line.str);
}

void gen_defvar(char *var) {
    str_concat(&line, "defvar gf@", var, NULL);

    ADDCODE(line.str);
}

void generate_code(FILE *destFile) {
    fputs(code.str, destFile);
    generator_end();
}