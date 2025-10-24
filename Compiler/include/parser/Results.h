// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef PARSER_RESUTLS_H
#define PARSER_RESUTLS_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "parser/ParserCore.h"

// ------------------------------------------------------------------------------------------------
// Public Types
// ------------------------------------------------------------------------------------------------

typedef enum ParserResultFlags {

	PARSER_RESULT_SUCCESS = 0x0000,

	/* Start section of warnings */
	PARSER_GENERAL_WARNING = 0x1000,
	PARSER_DEPRECATED_SYNTAX_WARNING,

	PARSER_RESULT_ERROR = 0x2000,
	PARSER_ERROR_INVALID_ARG,
	PARSER_ERROR_NO_MEMORY,
	PARSER_ERROR_INVALID_FILE,
	PARSER_ERROR_FILE_DATA_NULL,
	PARSER_ERROR_INVALID_STRATEGY,
	PARSER_ERROR_UNEXPECTED_TOKEN,
	PARSER_ERROR_SYNTAX_ERROR,
	PARSER_ERROR_UNTERMINATED_STRING,
	PARSER_ERROR_UNTERMINATED_COMMENT,
	PARSER_ERROR_INVALID_NUMBER_FORMAT,
	PARSER_ERROR_UNKNOWN_IDENTIFIER,
	PARSER_ERROR_UNCLOSED_PARENTHESIS,
	PARSER_ERROR_UNCLOSED_BRACE,
	PARSER_ERROR_UNCLOSED_BRACKET,
	PARSER_ERROR_INVALID_OPERATOR_USAGE,
	PARSER_ERROR_MISSING_SEMICOLON,
	PARSER_ERROR_REDECLARATION,

} ParserResultFlags;

#define CHECK_PARSER_RESULT(expr) \
    do { \
        ParserResult _res = (expr); \
        if (_res != PARSER_RESULT_SUCCESS) { \
            return _res; \
        } \
    } while(0)


// ------------------------------------------------------------------------------------------------
#endif // !PARSER_RESUTLS_H
// ------------------------------------------------------------------------------------------------