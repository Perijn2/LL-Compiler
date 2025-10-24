// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "parser/lexer/Lexer.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

struct Lexer_T {
    // ===== Input Management =====
    FileBuffer file;            // File buffer for reading source

    // ===== Current Position =====
    ParserSize line;            // Current line number
    ParserSize column;          // Current column number
    
    // ===== Token Lookahead =====
    LexerToken currentToken;    // Current token
    LexerToken peekToken;       // Lookahead token

    // ===== Error Tracking =====
    bool hasError;              // Flag indicating lexing errors
    const char* errorMessage;   // Last error message
    ParserSize errorLine;       // Line where error occurred
    ParserSize errorColumn;     // Column where error occurred

    // ===== Statistics (Optional) =====
    uint32_t tokenCount;        // Total tokens lexed
    uint32_t lineCount;         // Total lines in file

    // ===== Configuration/State =====
    FileBufferEncoding encoding; // Character encoding
    bool strictMode;            // Strict lexing rules

    // TODO: add preservation of whitespaces and comments

    // ===== LANGUAGE STRATEGY =====
    const LexerLanguageStrategy* strategy;  // Single pointer to strategy
};


/**
 * @brief Advance lexer cursor by one character
 *
 * @param lexer[in] Lexer handle
 * @return Current character or EOF
 */
PARSER_ATTR inline int PARSER_CALL LexerAdvance(
    Lexer lexer);

/**
 * @brief Peek at current character without advancing
 *
 * @param lexer[in] Lexer handle
 * 
 * @return Current character or EOF
 */
PARSER_ATTR int PARSER_CALL LexerPeek(
    Lexer lexer);

/**
 * @brief Peek at next character (lookahead)
 *
 * @param lexer[in] Lexer handle
 * @param offset[in] Offset from current position (1 = next char, 2 = char after that)
 * 
 * @return Character at offset or EOF
 */
PARSER_ATTR int PARSER_CALL LexerPeekOffset(
    Lexer lexer,
    size_t offset);

/**
 * @brief Check if at end of file
 *
 * @param lexer[in] Lexer handle
 *
 * @return true if at EOF, false otherwise
 */
PARSER_ATTR inline bool PARSER_CALL LexerIsAtEnd(
    const Lexer* lexer);


/**
 * @brief Check if at end of file
 *
 * @param lexer[in] Lexer handle
 *
 * @return true if at EOF, false otherwise
 */
PARSER_ATTR inline void PARSER_CALL LexerTrimWhitespaces(
    const Lexer lexer);


// ------------------------------------------------------------------------------------------------

#endif // !LEXER_INTERNAL_H

// ------------------------------------------------------------------------------------------------