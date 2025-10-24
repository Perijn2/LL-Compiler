// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef LEXER_H
#define LEXER_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "parser/ParserCore.h"

#include "FileBuffer.h"
#include "Token.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

PARSER_CORE_DEFINE_HANDLE(Lexer)

/**
 * @brief Check if lexeme is a keyword
 * 
 * @example: isKeyword("if") -> true, isKeyword("foo") -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsKeywordCallback)(
    const char* lexeme);

/**
 * @brief Check if character can start an identifier
 * 
 * @example: isIdentifierStart('a') -> true, isIdentifierStart('1') -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsIdentifierStartCallback)(
    uint8_t c);

/**
 * @brief Check if character can continue an identifier
 * 
 * @example: isIdentifierChar('_') -> true, isIdentifierChar('-') -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsIdentifierCharCallback)(
    uint8_t c);

/**
 * @brief Check if character is whitespace
 * 
 * @example: isWhitespace(' ') -> true, isWhitespace('a') -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsWhitespaceCallback)(
    uint8_t c);

/**
 * @brief Check if character starts a line comment
 * 
 * @example: isLineComment("//") -> true, isLineComment("/*") -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsLineCommentCallback)(
    const char* text,
    size_t length);

/**
 * @brief Check if character starts a block comment
 * 
 * @example: isBlockComment("/*") -> true, isBlockComment("//") -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsBlockCommentCallback)(
    const char* text,
    size_t length);

/**
 * @brief Check if character starts a string literal
 * 
 * @example: isStringStart('"') -> true, isStringStart('\'') -> false (for C)
 */
typedef bool (PARSER_PTR* PFN_LexerIsStringStartCallback)(
    uint8_t c);

/**
 * @brief Check if character starts a character literal
 * 
 * @example: isCharStart('\'') -> true (for C)
 */
typedef bool (PARSER_PTR* PFN_LexerIsCharStartCallback)(
    uint8_t c);

/**
 * @brief Check if character can start a number literal
 * 
 * @example: isNumberStart('1') -> true, isNumberStart('0') -> true
 */
typedef bool (PARSER_PTR* PFN_LexerIsNumberStartCallback)(
    uint8_t c);

/**
 * @brief Check if character can continue a number literal
 * 
 * @example: isNumberChar('5') -> true, isNumberChar('x') -> true (for hex), isNumberChar('a') -> false
 * 
 */
typedef bool (PARSER_PTR* PFN_LexerIsNumberCharCallback)(
    uint8_t c,
    int base);  // 2, 8, 10, 16

/**
 * @brief Check if character is punctuation
 * 
 * @example: isPunctuation('(') -> true, isPunctuation('a') -> false
 */
typedef bool (PARSER_PTR* PFN_LexerIsPunctuationCallback)(
    uint8_t c);

/**
 * @brief Check if lexeme is an operator
 * 
 * @param[in] lexeme The string to be checked
 * @param[in] length Size of @p lexeme in bytes
 * 
 * @example: isOperator("+") -> true, isOperator("++") -> true
 */
typedef bool (PARSER_PTR* PFN_LexerIsOperatorCallback)(
    const char* lexeme,
    size_t length);

/**
 * @brief Get operator type/precedence (C-specific)
 * 
 * @example: getOperatorType("&&") -> returns operator classification
 */
typedef uint32_t(PARSER_PTR* PFN_LexerGetOperatorTypeCallback)(
    const char* lexeme,
    size_t length);

typedef LexerToken(PARSER_PTR* PFN_LexerParseStringLiteral)(
    Lexer lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseCharLiteral)(
    Lexer lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseNumericLiteral)(
    Lexer lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseIdentifier)(
    Lexer lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseKeyword)(
    Lexer lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseOperator)(
    Lexer* lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseLineComment)(
    Lexer* lexer);

typedef LexerToken(PARSER_PTR* PFN_LexerParseBlockComment)(
    Lexer* lexer);

/**
 * @brief Language-specific lexer behavior
 *
 * @description Encapsulates all language-specific callback functions
 *              that determine how the lexer tokenizes source code.
 *              Different languages can share the same lexer core by
 *              using different strategy implementations.
 */
typedef struct LexerLanguageStrategy_T {
    const char* languageName;  // e.g., "C89", "C99", "C++11", "Python"

    // ===== Identifier Recognition =====
    PFN_LexerIsIdentifierStartCallback isIdentifierStart;
    PFN_LexerIsIdentifierCharCallback isIdentifierChar;

    // ===== Keyword Recognition =====
    PFN_LexerIsKeywordCallback isKeyword;

    // ===== Whitespace Handling =====
    PFN_LexerIsWhitespaceCallback isWhitespace;

    // ===== Comment Recognition =====
    PFN_LexerIsLineCommentCallback isLineComment;
    PFN_LexerIsBlockCommentCallback isBlockComment;

    // ===== Literal Recognition =====
    PFN_LexerIsStringStartCallback isStringStart;
    PFN_LexerIsCharStartCallback isCharStart;
    PFN_LexerIsNumberStartCallback isNumberStart;
    PFN_LexerIsNumberCharCallback isNumberChar;

    // ===== Operator Recognition =====
    PFN_LexerIsPunctuationCallback isPunctuation;
    PFN_LexerIsOperatorCallback isOperator;
    PFN_LexerGetOperatorTypeCallback getOperatorType;

    PFN_LexerParseStringLiteral parseStringLiteral;
    PFN_LexerParseCharLiteral parseCharLiteral;
    PFN_LexerParseNumericLiteral parseNumericalLiteral;
    
    PFN_LexerParseIdentifier parseIdentifier;

    PFN_LexerParseKeyword parseKeywords;

    PFN_LexerParseLineComment parseLineComments;
    PFN_LexerParseBlockComment parseBlockComments;

    void* userData;

} LexerLanguageStrategy;

typedef struct LexerCreateConfig_T {
    const LexerLanguageStrategy* strategy;
} LexerCreateConfig;

/**
 * @brief Creates a new lexer with file and language strategy
 *
 * @param file[in] File buffer to be lexed
 * @param cfg[in] Config for the lexer creation
 * @param lexer[out] Pointer to the lexer handle
 * 
 * @return ParserResult 
 *      PARSER_RESULT_SUCCES : Created lexer
 */
PARSER_ATTR ParserResult PARSER_CALL CreateLexer(
    FileBuffer file,
    LexerCreateConfig* cfg,
    Lexer* lexer);

/**
 * @brief Get next token from input stream
 *
 * @param lexer[in] Lexer handle
 * @param token[out] Pointer to the returned token 
 * 
 * @return ParserResult 
 *      PARSER_RESULT_SUCCES : Fetched token
 */
PARSER_ATTR ParserResult PARSER_CALL LexerNextToken(
    const Lexer lexer,
    LexerToken* token);


/**
 * @brief Get current token without advancing
 *
 * @param lexer[in] Lexer handle
 * 
 * @return Current token
 */
PARSER_ATTR LexerToken PARSER_CALL LexerCurrentToken(
    const Lexer lexer);

/**
 * @brief Get token at lookahead position
 *
 * @param lexer[in] Lexer handle
 * @param token[out] Pointer to the returned token 
 * 
 * @return ParserResult 
 *      PARSER_RESULT_SUCCES : Fetched token
 */
PARSER_ATTR ParserResult PARSER_CALL LexerLookAheadToken(
    const Lexer lexer,
    LexerToken* token);

// ===== ERROR HANDLING =====

/**
 * @brief Check if lexer encountered an error
 *
 * @param lexer[in] Lexer handle
 * 
 * @return true if error occurred, false otherwise
 */
PARSER_ATTR bool PARSER_CALL LexerHasError(
    const Lexer* lexer);

/**
 * @brief Get error message
 *
 * @param lexer[in] Lexer handle
 * 
 * @return Error message string or NULL
 */
PARSER_ATTR const char* PARSER_CALL LexerGetErrorMessage(
    const Lexer* lexer);

/**
 * @brief Get error location
 *
 * @param lexer Lexer handle
 * @param line Output line number
 * @param column Output column number
 */
PARSER_ATTR void PARSER_CALL LexerGetErrorLocation(
    const Lexer lexer,
    ParserSize line,
    ParserSize column);

// ===== CONFIGURATION =====

/**
 * @brief Enable/disable strict mode
 *
 * @param lexer Lexer handle
 * @param strict true for strict mode
 */
PARSER_ATTR void PARSER_CALL LexerSetStrictMode(
    Lexer lexer,
    bool strict);

/**
 * @brief Enable/disable whitespace preservation
 *
 * @param lexer Lexer handle
 * @param preserve true to preserve whitespace tokens
 */
PARSER_ATTR void PARSER_CALL Lexer_SetPreserveWhitespace(
    Lexer lexer,
    bool preserve);

/**
 * @brief Enable/disable comment preservation
 *
 * @param lexer Lexer handle
 * @param preserve true to preserve comment tokens
 */
PARSER_ATTR void PARSER_CALL Lexer_SetPreserveComments(
    Lexer lexer,
    bool preserve);

/**
 * @brief Set language strategy
 *
 * @param lexer Lexer handle
 * @param strategy Language-specific strategy
 */
PARSER_ATTR void PARSER_CALL Lexer_SetStrategy(
    Lexer lexer,
    const LexerLanguageStrategy* strategy);

// ===== STATISTICS =====

/**
 * @brief Get total tokens lexed
 *
 * @param lexer Lexer handle
 * @return Token count
 */
PARSER_ATTR uint32_t PARSER_CALL Lexer_GetTokenCount(
    const Lexer lexer);

/**
 * @brief Get current line number
 *
 * @param lexer Lexer handle
 * @return Line number
 */
PARSER_ATTR ParserSize PARSER_CALL Lexer_GetLine(
    const Lexer lexer);

/**
 * @brief Get current column number
 *
 * @param lexer Lexer handle
 * @return Column number
 */
PARSER_ATTR ParserSize PARSER_CALL Lexer_GetColumn(
    const Lexer lexer);

// ===== CLEANUP =====

/**
 * @brief Destroy lexer and clean up resources
 *
 * @param lexer[in] Lexer handle
 */
PARSER_ATTR void PARSER_CALL LexerDestroy(
    Lexer lexer);


// ------------------------------------------------------------------------------------------------

#endif // !LEXER_H

// ------------------------------------------------------------------------------------------------