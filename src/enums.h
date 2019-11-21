#ifndef _ENUMS_H
#define _ENUMS_H

typedef enum {
	KEYWORD_DEF =100,
	KEYWORD_ELSE,
	KEYWORD_IF,
	KEYWORD_NONE,
	KEYWORD_PASS,
	KEYWORD_RETURN,
	KEYWORD_WHILE,
	KEYWORD_AND,
	KEYWORD_OR,
	KEYWORD_IS,
	KEYWORD_NOT,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_PRINT,
	KEYWORD_INPUTS, // string
	KEYWORD_INPUTI, // int
	KEYWORD_INPUTF, // float
	KEYWORD_LEN,
	KEYWORD_SUBSTR,
	KEYWORD_ORD, // return ordinal value 
	KEYWORD_CHR, // return one char string

	//token types until emty line are equivalent to symbol types
	TOKEN_PLUS = 0 ,		// +
	TOKEN_MINUS,	// -
	TOKEN_MULTIPLICATION,	// *
	TOKEN_DIVISION,		// /
	TOKEN_IDIV,         // // 
	TOKEN_GREATER_THAN,	// >
	TOKEN_EQUAL,		// ==
	TOKEN_NOT_EQUAL,	// !=
	TOKEN_LESSER_EQUAL, // <=
	TOKEN_GREATER_EQUAL, // >=
	TOKEN_LESSER_THAN,	// <
	TOKEN_LEFT_BRACKET,		// (
	TOKEN_RIGHT_BRACKET,	// )
	TOKEN_IDENTIFIER ,
	TOKEN_INTEGER,
	TOKEN_DECIMAL,
	TOKEN_STRING,


	TOKEN_EOL,		// end of line
	TOKEN_EOF,		// end of file
	TOKEN_ASSIGN,		// =
	TOKEN_LEFT_SQUARE_BRACKET, // [
	TOKEN_RIGHT_SQUARE_BRACKET, // ]
	TOKEN_SINGLE_QUOTE,			// '
	TOKEN_DOUBLE_QUOTE,			// "
	TOKEN_INDENT,
	TOKEN_DEDENT,
	TOKEN_HASH,					// #
	TOKEN_PERCENT,				// %
	TOKEN_COLON,				// :
	TOKEN_COMMA,				// ,
} Token_type;

//enum is eqivalent of token type enum to make conversion from token type to symbol type easier
typedef enum {
	PLUS ,
	MINUS,
	MUL,
	DIV,
	IDIV,
	GTH,
	EQ,
	NEQ,
	LEQ,
	GEQ, 
	LTH,	
	LBRACKET,
	RBRACKET,
	
	// fromhere is the diference  compared to token enum !
	VALUE,
	ID,
	DOLLAR,
	NON_TERM,
} Symbol_enum; 

typedef enum {
	STATE_START =300,
	STATE_ERROR,
	STATE_COMMENT_SINGLE_LINE,
	STATE_COLON,
	STATE_RIGHT_SQUARE_BRACKET,
	STATE_LEFT_SQUARE_BRACKET,
	STATE_PERCENT,
	STATE_INDENTATION_CHECK,
	STATE_INDENT_ENDED,
	STATE_ID,
	STATE_KEYWORD,
	STATE_PLUS,
	STATE_MINUS,
	STATE_MULTIPLICATION,
	STATE_DIVISON,
	STATE_GREATER_THAN,
	STATE_LESSER_THAN,
	STATE_GREATER_EQUAL,
	STATE_LESSER_EQUAL,
	STATE_COMMA,
	STATE_NOT_EQUAL,
	STATE_LEFT_BRACKET,
	STATE_RIGHT_BRACKET,
	STATE_EQUAL,
	STATE_ASSIGN,
	STATE_EOL,
	STATE_EOF,
    STATE_STRING_START,
	STATE_STRING, 
	STATE_STRING_END,
	STATE_STRING_ESCAPE_SEQ,
	STATE_STR_ESCAPE_HEXA_1,
	STATE_STR_ESCAPE_HEXA_2,
	STATE_DOCSTRING_ENTER_1,
	STATE_DOCSTRING_ENTER_2,
	STATE_DOCSTRING_VALID,
	STATE_DOCSTRING_EXIT_1,
	STATE_DOCSTRING_EXIT_2,
	STATE_DOCSTRING_ESCAPE_SEQ,
	STATE_NUMBER,
	STATE_NUMBER_DECIMAL,
	STATE_NUMBER_EXPONENT,
	STATE_NUMBER_EXPONENT_VALID,
	STATE_EXPONENT_SIGN, 
} Scanner_state;



#endif
