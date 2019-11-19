#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "error.h"
#include <string.h>
#include "string_lib.h"
#include "typedefs.h"
#include <stdio.h>
#include <stdbool.h>

int generator_begin();
void generator_end();

void gen_print(unsigned n, Token token, ...);
void gen_var(char *var, Type, Value value, bool isGlobal);

void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool isGlobal);
void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool isGlobal);
void gen_defvar();
void generate_code(FILE *destFile);

void gen_pushs(Token token, bool isGlobal);
void gen_pops(char *var);
void gen_adds();
void gen_subs();
void gen_divs();
void gen_idivs();

void setScope(char *scope, bool isGlobal);

#endif