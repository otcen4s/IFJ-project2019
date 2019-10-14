#ifndef _SCANNER_H
#define _SCANNER_H

#include "enums.h"

typedef union {
	char *string;
	int integer;
	double decimal;
} Token_attribute;

typedef struct {
	Token_type type;
	Token_attribute attribute;
} Token;

#endif
