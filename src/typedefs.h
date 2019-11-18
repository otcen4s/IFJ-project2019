

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