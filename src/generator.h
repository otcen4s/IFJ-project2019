#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "error.h"
#include <string.h>
#include "string_lib.h"

tString code;
int error;

int generator_begin();
int generator_end();

void gen_print(char *string);

#endif