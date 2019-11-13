#include "generator.h"

#define ADDCODE(line) \
do { \
str_append(&code, line); \
str_append(&code, "\n"); \
 } while (0) \

tString code;
tString line;

int error;

int generator_begin() {
    if ((error = str_init(&code))) {
        return error;
    }

    if ((error = str_init(&line))) {
        return error;
    }

    // generate header
    ADDCODE(".IFJcode19");
    ADDCODE("label $main");
    ADDCODE("createframe");
    ADDCODE("pushframe");

    return NO_ERROR;
}

int generator_end() {

    str_destroy(&code);
    str_destroy(&line);

    return NO_ERROR;
}

void gen_var(char *var, Type type, Value value) {
    str_concat(&line, "defvar GF@", var, NULL);
    ADDCODE(line.str);

    if (type == TYPE_STRING) {
        str_concat(&line, "move GF@", var, " string@", value.string, NULL);
    } else if (type == TYPE_INT) {
        str_concat(&line, "move GF@", var, " int@", value.string, NULL);
    } else if (type == TYPE_FLOAT) {
        char temp[128];
        sprintf(temp, "%a", value.decimal);
        str_concat(&line, "move GF@", var, " float@", temp, NULL);
    } else {
        str_concat(&line, "move GF@", var, " nil@nil", NULL);
    }

    ADDCODE(line.str);
}

void gen_arit(char *instruct, Type type, char *var, Value symb1, Value symb2) {
    if (type == TYPE_FLOAT) {
        char temp[128];
        sprintf(temp, " float@%a float@%a", symb1.decimal, symb2.decimal);

        str_concat(&line, instruct, " gf@", var, temp, NULL);
    } else {
        if (type == TYPE_INT) {
            str_concat(&line, instruct, " gf@", var, " int@", symb1.string, " int@", symb2.string, NULL);
        } else if (type == TYPE_STRING) {
            str_concat(&line, instruct, " gf@", var, " string@", symb1.string, " string@", symb2.string, NULL);
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