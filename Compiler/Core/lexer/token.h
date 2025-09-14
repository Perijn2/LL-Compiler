//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: token.h
* Moodified: 18/03/2025
*/
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#pragma once
#ifndef _TOKEN_H_
#define _TOKEN_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdint.h>

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

typedef enum {
	//
	TOK_INVALID = 0,

	TOK_EOF,
	TOK_HASH,

	TOK_INTEGER,
	TOK_FLOAT,
	TOK_IDENTIFIER,
    TOK_STRING,
    TOK_CHAR,
	TOK_KEYWORD,
	TOK_TYPEDEF,

	TOK_OPERATOR,

	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_LANGLE,
	TOK_RANGLE,
	TOK_SEMICOLON,
	TOK_COMMA,
	TOK_DOT,
	TOK_COLON,
	TOK_QUESTION,
} token_type_t;

typedef enum {
    /* invalid */
    OP_INVALID = 0,

    /* arithmetic (binary) */
    OP_ADD,              // +
    OP_MIN,              // -
    OP_MUL,              // *
    OP_DIV,              // /
    OP_MOD,              // %

    /* unary */
    OP_PLUS,             // +  (unary)
    OP_MINUS,            // -  (unary)
    OP_INC,              // ++
    OP_DEC,              // --
    OP_BIT_NOT,          // ~
    OP_LOG_NOT,          // !

    /* bitwise */
    OP_BIT_AND,          // &
    OP_BIT_OR,           // |
    OP_BIT_XOR,          // ^
    OP_SHL,              // <<
    OP_SHR,              // >>

    /* logical */
    OP_LOG_AND,          // &&
    OP_LOG_OR,           // ||

    /* comparison */
    OP_EQ,               // ==
    OP_NEQ,              // !=
    OP_LESS,             // <
    OP_GREATER,          // >
    OP_LESS_EQUAL,       // <=
    OP_GREATER_EQUAL,    // >=

    /* assignment */
    OP_ASSIGN,           // =
    OP_ADD_ASSIGN,       // +=
    OP_MIN_ASSIGN,       // -=
    OP_MUL_ASSIGN,       // *=
    OP_DIV_ASSIGN,       // /=
    OP_MOD_ASSIGN,       // %=,
    OP_SHL_ASSIGN,       // <<=
    OP_SHR_ASSIGN,       // >>=
    OP_AND_ASSIGN,       // &=   (bitwise)
    OP_OR_ASSIGN,        // |=
    OP_XOR_ASSIGN,       // ^=

    /* misc / punctuators often treated as ops */
    OP_DOT,              // .
    OP_ARROW,            // ->
    OP_ELLIPSIS,         // ...
    OP_QUESTION,         // ?
    OP_COLON,            // :
    OP_COMMA,            // ,
    OP_HASH,             // #
    OP_HASHHASH          // ##
} operator_subtype_t;
//------------------------------------------------------------------------------
// structs
//------------------------------------------------------------------------------

typedef struct token {
	token_type_t type;
	operator_subtype_t subtype;   // tighter typing
	const char* text;             // slice into source
	size_t len;                // bytes
	uint32_t line;                // 1-based
	union {
		uint64_t intValue;
		double   floatValue;
	};
	bool bol_hash;
} token_t;

//------------------------------------------------------------------------------

#endif // !_TOKEN_H_

//------------------------------------------------------------------------------