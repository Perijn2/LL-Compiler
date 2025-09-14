//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: Lexer.h
* Moodified: 16/04/2025
*/
//------------------------------------------------------------------------------

#include "lexer.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const SymbolDef symbols[] = {
		{"(", TOK_LPAREN}, {")", TOK_RPAREN},
		{"{", TOK_LBRACE}, {"}", TOK_RBRACE},
		{"[", TOK_LBRACKET}, {"]", TOK_RBRACKET},
		{"<", TOK_LANGLE}, {">", TOK_RANGLE},
		{";", TOK_SEMICOLON}, {",", TOK_COMMA},
		{NULL, TOK_INVALID} // Sentinel
};

// Sorted by text length then alphabetically
static const OperatorDef OPERATORS[] = {
	{"+", OP_ADD,    1},
	{"-", OP_MIN,    1},
	{"*", OP_MUL,    1},
	{"/", OP_DIV,    1},
	{"%", OP_MOD,    1},
	{"=", OP_ASSIGN, 1},
	{"==", OP_EQ,    2},
	{"!=", OP_NEQ,    2},
	{"--", OP_DEC,    2},
	{"++", OP_INC,    2},

	// TODO:
	/*{">=", OP_EQ,    2},
	{"<=", OP_NE,    2},
	{">", OP_EQ,    2},
	{"<", OP_NE,    2},*/
	// ... add all other operators
	{NULL, OP_INVALID, 0} // Sentinel
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

char* subString(const char* source, uint64_t length) {
	char* sub = (char*) malloc(length + 1);  // +1 for null terminator
	if (!sub) return NULL;
	
	strncpy(sub, source, length);
	sub[length] = '\0';  // Ensure null-termination
	return sub;
}

uint8_t isOctalDigit(char c)
{
	return c >= '0' && c <= '7';
}

uint8_t isBinaryDigit(char c)
{
	return c == '0' || c == '1';
}

uint8_t isDecimalDigit(char c)
{
	return isdigit(c);
}

uint8_t isHexadecimalDigit(char c)
{
	char lc = tolower(c);
	return isdigit(c) || (lc >= 'a' && lc <= 'f');
}

uint8_t isNumberStart(Lexer* lex) {
	char c = lex->source[lex->pos];

	// Check for digits (most common case)
	if (isdigit(c) && c != '0') return 1;

	// Check for hex/binary/octal prefixes
	if (c == '0' && lex->pos + 1 < lex->size) {
		char next = tolower(lex->source[lex->pos + 1]);
		if (next == 'x' || next == 'b') return 1;  // 0x or 0b
		if (isdigit(next)) return 1;  // Octal (leading 0)
	}

	// Check for decimal point followed by digit
	if (c == '.' && lex->pos + 1 < lex->size) {
		return isdigit(lex->source[lex->pos + 1]);
	}

	return 0;
}



static token_t getSymbolType(char c)
{
	// Compare against known symbols
	for (const SymbolDef* def = symbols; def->symbol; def++) {
		if (c == def->symbol[0]) {
			return def->type;
		}
	}
	return TOK_INVALID;
}

static uint32_t hashOperator(const char* str, uint64_t len) {
	const uint32_t FNV_PRIME = 0x01000193;
	uint32_t hash = 0x811C9DC5;
	for (uint64_t i = 0; i < len; i++) {
		hash ^= (uint8_t)str[i];
		hash *= FNV_PRIME;
	}
	return hash;
}

char* trimComments(char* src)
{
	char* ret = src;
	char* dst = src;
	while (*src) {
		if (*src == '/' && src[1] == '*') {
			src += 2;
			while (!(*src == '*' && src[1] == '/')) src++;
			src += 2;
		}
		else if (*src == '/' && src[1] == '/') {
			while (*src != '\n' && *src != '\0') src++;
		}
		else {
			*(dst++) = *(src++);
		}
	}
	*dst = '\0';
	return ret;
}

void advanceLexerCallback(Lexer* lex, LexerCallback callback)
{
	char c;
	while (lex->pos < lex->size)
	{
		advanceCharacter(lex);
		c = lex->source[lex->pos];
		if (!callback(c)) break;
	}
}

uint8_t advanceCharacter(Lexer* lex)
{
	if (lex->pos >= lex->size) return 1;

	if (lex->source[lex->pos] == '\n') {
		lex->line++;
		lex->bol = lex->pos + 1;  // Next line starts after '\n'
	}

	lex->pos++;  // Advance to next character
	return 0;
}

void trimWhitespaces(Lexer* lex)
{
	if (lex->pos >= lex->size) return;

	while (isspace(lex->source[lex->pos]))
	{
		if (advanceCharacter(lex)) return;
	}
}

Token* tokenizeOperator(Lexer* lex) {
	// Try matching longest possible operators first (max 4 chars)
	uint64_t max_len = (lex->size - lex->pos) > 4 ? 4 : (lex->size - lex->pos);

	for (uint64_t len = max_len; len >= 1; len--) {
		uint32_t h = hashOperator(lex->source + lex->pos, len);

		// Linear search through operators of this length
		for (const OperatorDef* op = OPERATORS; op->text; op++) {
			if (op->length != len) continue;

			// Compare hash first as a fast filter
			if (h == hashOperator(op->text, len)) {
				// Verify exact match (hash collision check)
				if (memcmp(lex->source + lex->pos, op->text, len) == 0) {
					
					Token* newToken = (Token*)malloc(sizeof(Token));
					if (!newToken) return NULL;

					newToken->type = TOK_OPERATOR; //op->type;
					newToken->subtype = op->type;
					newToken->length = op->length;

					return newToken;
				}
			}
		}
	}
	return NULL; // No operator found
}

Token* tokenizeNumbers(Lexer* lex)
{
	uint64_t pos = lex->pos;
	uint64_t line = lex->line;

	// Check EOF
	if (lex->pos >= lex->size) return NULL;

	char c = lex->source[lex->pos];
	if (!isNumberStart(lex)) return NULL;

	uint64_t result = 0;
	char* numberSrc = NULL;

	if (c == '0')
	{
		// Special number notations like binary, hexadecimal and octal
		if (advanceCharacter(lex)) return NULL;
		c = lex->source[lex->pos];

		if (tolower(c) == 'x')
		{
			// Hexadecimals lexing
			advanceLexerCallback(lex, isHexadecimalDigit);
			if ((lex->pos - pos) <= 2) return NULL;

			numberSrc = subString(lex->source+pos, lex->pos-pos);
			result = hexadecimalToInteger(numberSrc+2);
		}
		else if (tolower(c) == 'b')
		{
			// Binary lexing
			advanceLexerCallback(lex, isBinaryDigit);
			if ((lex->pos - pos) <= 2) return NULL;

			numberSrc = subString(lex->source + pos, lex->pos - pos);
			result = binaryToInteger(numberSrc+2);
		}
		else if (isOctalDigit(c)) {
			advanceLexerCallback(lex, isOctalDigit);
			numberSrc = subString(lex->source + pos, lex->pos - pos);
			result = octalToInteger(numberSrc+1);
		}
		else {
			// Just a null number
			numberSrc = subString(lex->source + pos, lex->pos - pos);    
		}

		Token* newToken = (Token*)malloc(sizeof(Token));
		if (!newToken) {
			free(numberSrc);
			return NULL;
		}

		newToken->type = TOK_INTEGER;
		newToken->length = lex->pos - pos;
		newToken->text = numberSrc;
		newToken->line = line;
		newToken->intValue = result;

		return newToken;
	}
	else if (c)
	{
		// Normal number notation

		// TODO: implement the rest of the number features
		if (c == '.')
		{
			advanceCharacter(lex);
		}
		advanceLexerCallback(lex, isDecimalDigit);
		
		numberSrc = subString(lex->source + pos, lex->pos - pos);
		result = decimalToInteger(numberSrc);

		Token* newToken = (Token*)malloc(sizeof(Token));
		if (!newToken) {
			free(numberSrc);
			return NULL;
		}

		newToken->type = TOK_INTEGER;
		newToken->length = lex->pos - pos;
		newToken->text = numberSrc;
		newToken->line = line;
		newToken->intValue = result;

		return newToken;
	}
}

Token* tokenizeIdentifier(Lexer* lex)
{
	uint64_t pos = lex->pos;

	// Check EOF
	if (lex->pos >= lex->size) return NULL;

	// Validate first character (must be [a-zA-Z_])
	char c = lex->source[lex->pos];
	if (!isalpha(c) && c != '_') return NULL;

	// Read identifier chars [a-zA-Z0-9_]
	do {
		if (advanceCharacter(lex)) break; // Exit on EOF/error
		c = lex->source[lex->pos]; //lex->pos < lex->size ? lex->source[lex->pos] : '\0';
	} while (isalnum(c) || c == '_');

	// Allocate token
	Token* newToken = (Token*)malloc(sizeof(Token));
	if (!newToken) return NULL;

	// Initialize token
	newToken->type = TOK_IDENTIFIER;
	newToken->length = lex->pos - pos;
	newToken->text = subString(lex->source + pos, newToken->length);
	newToken->line = lex->line;

	return newToken;
}

Token* peekNext(Lexer* lex)
{

}

Token* consumeNext(Lexer* lex)
{

}

Token* nextToken(Lexer* lex)
{
	// Trim away all the whitespaces
	trimWhitespaces(lex);

	if (lex->pos >= lex->size) return NULL;
	uint64_t line = lex->line;

	char c = lex->source[lex->pos];
	switch (c)
	{
	case 'a'...'z': case 'A'...'Z': case '_':
	{
		Token* token = tokenizeIdentifier(lex);

		keyword_t isKeyword = search(lex->keywords, token->text);
		if (isKeyword)
		{
			token->type = TOK_KEYWORD;
			token->subtype = isKeyword;
		}

		return token;
	}
	case '0'...'9':
	{
		return tokenizeNumbers(lex);

		break;
	}
	case '\'': case '"':
	{

		break;
	}
	case '{': case '}': case '[': case ']':
	case '(': case ')': case '<': case '>':
	case ';':
	{
		token_t symbolType = getSymbolType(c);
		if (symbolType == TOK_INVALID) {
			return NULL;
		}

		char* text = (char*)malloc(2);
		if (!text) return NULL;
		text[0] = c;
		text[1] = '\0';

		Token* newToken = (Token*)malloc(sizeof(Token));
		if (!newToken) {
			free(text);
			return NULL;
		}

		newToken->type = symbolType;
		newToken->text = text;
		newToken->length = 1;
		newToken->line = line;

		advanceCharacter(lex);

		return newToken;
	}
	case '+': case '-': case '/': case '*': case '=':
	case '%': case '!':
	{
		Token* newToken = tokenizeOperator(lex);

		for (uint8_t i = 0; i < newToken->length; i++)
		{
			advanceCharacter(lex);
		}

		return newToken;
	}
	default:
		break;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------