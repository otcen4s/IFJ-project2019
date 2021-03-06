/*
* Project           : IFJ19 Compiler
* File              : generator.h
* Author            : Samuel Dudík
*/

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "error.h"
#include <string.h>
#include "string_lib.h"
#include "typedefs.h"
#include "stack.h"
#include <stdio.h>
#include <stdbool.h>

int generator_begin();
void generator_end();

void gen_print(bool global, Token token);
void gen_print_space();
void gen_print_end();
void gen_defvar(char *varName, bool global);
void gen_instruct(const char *instruct);

void gen_double_symb(char *instruct, Type type, char *var, Value symb1, Value symb2, bool global);
void gen_single_symb(char *instruct, Type type, char *var, Value symb, bool global);
void generate_code(FILE *destFile);

void gen_pushs(Token token, bool global);
void gen_pops(char *var, bool global);
void gen_stack(const char *instruct);

void gen_move_retval(char *varName, bool global);
void gen_func_def_start(char *funcName);
void gen_func_def_add_param(char *paramName);
void gen_func_def_return();
void gen_func_def_end();

void gen_func_call_start();
void gen_func_call_add_param(Token token, bool global);
void gen_func_call_end(char *funcName, char *varName, bool global);
void gen_func_builtin_call_end(char *funcName);

void gen_if_start();
void gen_if_end();
void gen_else_start();
void gen_else_end();

void gen_while_start();
void gen_while_eval();
void gen_while_end();

void gen_inputs();
void gen_inputi();
void gen_inputf();

void gen_lts();
void gen_ltes();
void gen_gts();
void gen_gtes();
void gen_eqs();
void gen_neqs();

void gen_func_start(char *funcName);

const char *replace_space(char *string);

#endif