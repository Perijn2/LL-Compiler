// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <stdbool.h>

#include "parser/ParserCore.h"

// ------------------------------------------------------------------------------------------------
// Public Types
// ------------------------------------------------------------------------------------------------

PARSER_CORE_DEFINE_HANDLE(LexerToken)

typedef enum TokenTypeFlags {
	TOKEN_TYPE_NONE = 0x0000,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_LITERAL,
	TOKEN_TYPE_OPERATOR,
	TOKEN_TYPE_PUNCTUATION,
	TOKEN_TYPE_PREPROCESSOR,
	TOKEN_TYPE_COMMENT,
	TOKEN_TYPE_EOF,
} TokenTypeFlags;

typedef enum TokenOperatorTypeFlags {
	OPERATOR_TYPE_NONE = 0x0000,
	OPERATOR_TYPE_ARITHMETIC,
	OPERATOR_TYPE_LOGICAL,
	OPERATOR_TYPE_COMPARISON,
	OPERATOR_TYPE_ASSIGNMENT,
	OPERATOR_TYPE_BITWISE,
	OPERATOR_TYPE_UNARY,
	OPERATOR_TYPE_TERNARY,
} TokenOperatorTypeFlags;

/**
* @brief Arithmetic operator flags
*/
typedef enum TokenArithmeticOperatorFlags {
	ARITHMETIC_OPERATOR_NONE = 0x0000,

	/**
	* @brief Addition operator (+)
	*/
	ARITHMETIC_OPERATOR_ADD,

	/**
	* @brief Substraction operator (-)
	*/
	ARITHMETIC_OPERATOR_SUBTRACT,

	/**
	* @brief Multiplication operator (*)
	*/
	ARITHMETIC_OPERATOR_MULTIPLY,

	/**
	* @brief Division operator (/)
	*/
	ARITHMETIC_OPERATOR_DIVIDE,

	/**
	* @brief Modules operator (%)
	*/
	ARITHMETIC_OPERATOR_MODULO,
} TokenArithmeticOperatorFlags;

/**
* @brief Logical operator flags
*/
typedef enum TokenLogicalOperatorFlags {
	LOGICAL_OPERATOR_NONE = 0x0000,

	/**
	* @brief Logic AND operator (&&)
	*/
	LOGICAL_OPERATOR_AND,

	/**
	* @brief Logic OR operator (||)
	*/
	LOGICAL_OPERATOR_OR,

	/**
	* @brief Logic NOT operator (!)
	*/
	LOGICAL_OPERATOR_NOT,
} TokenLogicalOperatorFlags;

typedef enum TokenComparisonOperatorFlags {
	COMPARISON_OPERATOR_NONE = 0x0000,

	/**
	* @brief Comparison equal operator (==)
	*/
	COMPARISON_OPERATOR_EQUAL,

	/**
	* @brief Comparison not equal operator (!=)
	*/
	COMPARISON_OPERATOR_NOT_EQUAL,

	/**
	* @brief Comparison less operator (<)
	*/
	COMPARISON_OPERATOR_LESS,

	/**
	* @brief Comparison greater operator (>)
	*/
	COMPARISON_OPERATOR_GREATER,

	/**
	* @brief Comparison less equal operator (<=)
	*/
	COMPARISON_OPERATOR_LESS_EQUAL,

	/**
	* @brief Comparison greater equal operator (>=)
	*/
	COMPARISON_OPERATOR_GREATER_EQUAL,
} TokenComparisonOperatorFlags;

typedef enum TokenAssignmentOperatorFlags {
	ASSINGMENT_OPERATOR_NONE = 0x0000,

	/**
	* @brief Assingment assign operator (=)
	*/
	ASSINGMENT_OPERATOR_ASSIGN,

	/**
	* @brief Assingment add assign operator (+=)
	*/
	ASSINGMENT_OPERATOR_ADD_ASSIGN,

	/**
	* @brief Assingment substract assign operator (-=)
	*/
	ASSINGMENT_OPERATOR_SUBTRACT_ASSIGN,

	/**
	* @brief Assingment multiply assign operator (*=)
	*/
	ASSINGMENT_OPERATOR_MULTIPLY_ASSIGN,

	/**
	* @brief Assingment division assign operator (/=)
	*/
	ASSINGMENT_OPERATOR_DIVIDE_ASSIGN,

	/**
	* @brief Assingment modulo assign operator (%=)
	*/
	ASSINGMENT_OPERATOR_MODULO_ASSIGN,
} TokenAssignmentOperatorFlags;

typedef enum TokenBitwiseOperatorFlags {
	BITWISE_OPERATOR_NONE = 0x0000,

	/**
	* @brief Bitwise AND operator (&)
	*/
	BITWISE_OPERATOR_AND,

	/**
	* @brief Bitwise OR operator (|)
	*/
	BITWISE_OPERATOR_OR,

	/**
	* @brief Bitwise XOR operator (^)
	*/
	BITWISE_OPERATOR_XOR,

	/**
	* @brief Bitwise NOT operator (~)
	*/
	BITWISE_OPERATOR_NOT,

	/**
	* @brief Bitwise bitshift left operator (<<)
	*/
	BITWISE_OPERATOR_SHL,

	/**
	* @brief Bitwise bitshift right operator (>>)
	*/
	BITWISE_OPERATOR_SHR,
} TokenBitwiseOperatorFlags;

typedef enum TokenUnaryOperatorFlags {
	UNARY_OPERATOR_NONE = 0x0000,

	UNARY_OPERATOR_INCREMENT,
	UNARY_OPERATOR_DECREMENT,
	UNARY_OPERATOR_PLUS,
	UNARY_OPERATOR_MINUS,
} TokenUnaryOperatorFlags;

typedef enum TokenTernaryOperatorFlags {
	TERNARY_OPERATOR_NONE = 0x0000,
	TERNARY_OPERATOR_CONDITIONAL,
} TokenTernaryOperatorFlags;

typedef enum TokenLiteralTypeFlags {
	LITERAL_TYPE_NONE = 0x0000,
	LITERAL_TYPE_INTEGER,
	LITERAL_TYPE_FLOAT,
	LITERAL_TYPE_STRING,
	LITERAL_TYPE_CHAR,
	LITERAL_TYPE_BOOLEAN,
	LITERAL_TYPE_NULL,
	LITERAL_TYPE_MAX_VALUE,
} TokenLiteralTypeFlags;


PARSER_ATTR inline bool PARSER_CALL IsTokenKeyword(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenIdentifier(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenLiteral(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenPunctuation(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenComment(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenEOF(const LexerToken token);

PARSER_ATTR inline bool PARSER_CALL IsTokenArithOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenLogicalOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenComparisonOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenAssignmentOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenBitwiseOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenUnaryOperator(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenTernaryOperator(const LexerToken token);

PARSER_ATTR inline bool PARSER_CALL IsTokenLiteralInteger(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenLiteralFloat(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenLiteralString(const LexerToken token);
PARSER_ATTR inline bool PARSER_CALL IsTokenLiteralChar(const LexerToken token);

PARSER_ATTR const char* PARSER_CALL TokenTypeToString(const LexerToken token);
PARSER_ATTR const char* PARSER_CALL TokenOperatorToString(const LexerToken token);
PARSER_ATTR const char* PARSER_CALL TokenLiteralTypeToString(const LexerToken token);


// ------------------------------------------------------------------------------------------------
#endif // !LEXER_TOKEN_H
// ------------------------------------------------------------------------------------------------