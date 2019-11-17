#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "error.h"
#include <string.h>
#include "string_lib.h"
#include <stdio.h>
#include <stdbool.h> 

typedef union {
    char* string;
	double decimal;
} Value;

typedef enum 
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NIL
} Type;

int generator_begin();
void generator_end();

void gen_print(char *string);
void gen_var(char *var, Type, Value value, bool isGlobal);

void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool isGlobal);
void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool isGlobal);
void gen_defvar();
void generate_code(FILE *destFile);

void setScope(char *scope, bool isGlobal);

#endif