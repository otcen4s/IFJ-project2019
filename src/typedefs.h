#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

#include "enums.h"
#include  "string_lib.h"

typedef union {
	tString string;
	int integer;
	double decimal;
} Token_attribute;

typedef struct {
	Token_type type;
	Token_attribute attribute;
} Token;

// TODO maybe delete below
typedef union {
    char* string;
	double decimal;
    int integer;
} Value;

typedef enum 
{
    TYPE_STRING,
    TYPE_DECIMAL,
    TYPE_INTEGER,
    TYPE_VARIABLE
} Type;

#endif