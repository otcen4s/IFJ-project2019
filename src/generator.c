#include "generator.h"

#define ADDCODE(line) \
do { \
str_append(&code, line); \
str_append(&code, "\n"); \
 } while (0) \

int generator_begin() {

    if ((error = str_init(&code))) {
        return error;
    }

    // generate header
    ADDCODE(".IFJcode19");
    ADDCODE("label $main");
    ADDCODE("createframe");
    ADDCODE("pushframe");

    gen_print("meduna\\032je\\032sef");

    printf("KUKAJ:\n%s\n", code.str);

    return NO_ERROR;
}

int generator_end() {

    str_destroy(&code);

    return NO_ERROR;
}

void gen_print(char *string) {
    char printedStr[strlen(string) + 50];
    strcpy(printedStr, "WRITE string@");
    strcat(printedStr, string);
    ADDCODE(printedStr);
}