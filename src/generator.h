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

void gen_print(unsigned n, bool global, Token token, ...);
void gen_var(char *varName, bool global);
void gen_instruct(const char *instruct);

void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool global);
void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool global);
void gen_defvar();
void generate_code(FILE *destFile);

void gen_pushs(Token token, bool global);
void gen_pops(char *var, bool global);
void gen_stack(const char *instruct);

void gen_if_start();
void gen_if_end();
void gen_else_start();
void gen_else_end();

void gen_while_start();
void gen_while_end();

void gen_inputs();
void gen_inputi();
void gen_inputf();

void gen_lts();
void gen_gts();
void gen_eqs();

const char *replace_space(char *string);

#endif