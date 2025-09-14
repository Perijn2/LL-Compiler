//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: Lexer.h
* Modified: 16/04/2025
*/
//------------------------------------------------------------------------------

#pragma once
#ifndef _LEXER_H_
#define _LEXER_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdint.h>

/*–––– Project headers ––––*/
#include "keywords.h"

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef uint8_t(*LexerCallback) (char c);



typedef struct {
	const char* source;
	uint64_t size;
	uint64_t pos;
	uint64_t line;
	uint64_t bol;
	
	TrieNode* keywords;
} lexer;

typedef struct {
	const char* symbol;
	token_t type;
} SymbolDef;

// Operator definition table
typedef struct {
	const char* text;    // Operator string
	Operators type;      // Corresponding token
	uint8_t length;      // Operator length (1-4 chars)
} OperatorDef;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

//void handlePreprocs(Lexer* lex);

char* trimComments(char* src);
char* subString(const char* source, uint64_t length);

uint8_t isOctalDigit(char c);
uint8_t isBinaryDigit(char c);
uint8_t isDecimalDigit(char c);
uint8_t isHexadecimalDigit(char c);
uint8_t isNumberStart(Lexer* lex);

uint64_t octalToInteger(const char* src);
uint64_t binaryToInteger(const char* src);
uint64_t hexadecimalToInteger(const char* src);
uint64_t decimalToInteger(const char* src);

static token_t getSymbolType(char c);
static uint32_t hashOperator(const char* str, uint64_t len);

void advanceLexerCallback(Lexer* lex, LexerCallback callback);
uint8_t advanceCharacter(Lexer* lex);

void trimWhitespaces(Lexer* lex);

Token* tokenizeOperator(Lexer* lex);
Token* tokenizeNumbers(Lexer* lex);
Token* tokenizeIdentifier(Lexer* lex);

Token* peekNext(Lexer* lex);
Token* consumeNext(Lexer* lex);

Token* nextToken(Lexer* lex);


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // !_LEXER_H_

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
