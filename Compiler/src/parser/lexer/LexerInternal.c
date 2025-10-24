// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "LexerInternal.h"

#include "parser/Results.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

PARSER_ATTR static LexerToken PARSER_CALL Lexer_GenerateNextToken(Lexer* lexer);



PARSER_ATTR ParserResult PARSER_CALL CreateLexer(
    FileBuffer file,
    LexerCreateConfig* cfg,
    Lexer* lexer)
{
    if (!file || !cfg || !lexer)
        return PARSER_ERROR_INVALID_ARG;

    Lexer hdl = PARSER_MALLOC(sizeof(struct Lexer_T), NULL);
    if (!hdl)
        return PARSER_ERROR_NO_MEMORY;

   
    hdl->file = file;
    hdl->column = 0;
    hdl->line = 0;


    // TODO: Implement lookahead allocation
    hdl->currentToken = NULL;
    hdl->peekToken = NULL;

    hdl->encoding = GetFileBufferEncoding(file);
    hdl->strictMode = false;

    // Setting the language strategy. The language strategy is essentially the 
    // syntax of the to-be-compiled file
    if (!cfg->strategy) {
        return PARSER_ERROR_INVALID_STRATEGY;
    }
    hdl->strategy = cfg->strategy;

    *lexer = hdl;

    return PARSER_RESULT_SUCCESS;
}

PARSER_ATTR ParserResult PARSER_CALL LexerNextToken(
    const Lexer lexer,
    LexerToken* token)
{
    // ===== INPUT VALIDATION =====
    if (!lexer) {
        return PARSER_ERROR_INVALID_ARG;
    }

    if (!token) {
        return PARSER_ERROR_INVALID_ARG;
    }

    // ===== CHECK FOR ERROR STATE =====
    if (lexer->hasError) {
        return PARSER_RESULT_ERROR;
    }

    // ===== SHIFT TOKEN WINDOW (lazy lookahead) =====
    // In lazy lexing with 2-token lookahead:
    // currentToken -> return to parser
    // peekToken -> becomes new currentToken
    // generate new peekToken

    *token = lexer->currentToken;

    // Shift lookahead window
    lexer->currentToken = lexer->peekToken;
    lexer->peekToken = Lexer_GenerateNextToken(lexer);

    // Update statistics
    lexer->tokenCount++;

    return PARSER_RESULT_SUCCESS;
}

/**
 * @brief Internal: Generate the next token from input stream
 * @note This is STATIC - called only by Lexer_NextToken
 */
PARSER_ATTR static LexerToken PARSER_CALL Lexer_GenerateNextToken(Lexer lexer) {
    //LexerToken token = { 0 };

    // ===== SKIP WHITESPACE AND COMMENTS =====
    LexerTrimWhitespaces(lexer);
    

    // ===== SET TOKEN LOCATION =====
    //token.line = lexer->line;
    //token.column = lexer->column;

    // ===== TOKENIZE BASED ON CHARACTER TYPE =====
    int32_t byte = PeekFileBuffer(lexer->file);
    char c = (char)byte;

    // ===== STRING LITERAL =====
    if (lexer->strategy->isStringStart(c)) {
        return Lexer_ParseStringLiteral(lexer);
    }

    // ===== CHARACTER LITERAL =====
    if (lexer->strategy->isCharStart(c)) {
        return Lexer_ParseCharLiteral(lexer);
    }

    // ===== NUMERIC LITERAL =====
    if (lexer->strategy->isNumberStart(c)) {
        return Lexer_ParseNumericLiteral(lexer);
    }

    // ===== IDENTIFIER OR KEYWORD =====
    if (lexer->strategy->isIdentifierStart(c)) {
        return Lexer_ParseIdentifierOrKeyword(lexer);
    }

    // ===== OPERATOR =====
    if (lexer->strategy->isOperator((const char*)lexer->file->Cursor.cur, 2)) {
        return Lexer_ParseOperator(lexer);
    }

    // ===== PUNCTUATION =====
    if (lexer->strategy->isPunctuation(c)) {
        token.flags = TOKEN_TYPE_PUNCTUATION;
        token.lexeme = (const char*)lexer->file->Cursor.cur;
        FileBuffer_Advance(lexer->file);
        lexer->column++;
        return token;
    }

    // ===== UNKNOWN CHARACTER - ERROR =====
    lexer->hasError = true;
    lexer->errorMessage = "Unexpected character";
    lexer->errorLine = lexer->line;
    lexer->errorColumn = lexer->column;

    token.flags = TOKEN_TYPE_ERROR;
    token.lexeme = (const char*)lexer->file->Cursor.cur;
    FileBuffer_Advance(lexer->file);

    return token;
}


PARSER_ATTR inline int PARSER_CALL LexerAdvance(
    Lexer lexer)
{
    if (!lexer)
        return PARSER_ERROR_INVALID_ARG;

    return AdvanceFileBuffer(lexer->file);
   
}

PARSER_ATTR int PARSER_CALL LexerPeek(
    Lexer lexer)
{

}

PARSER_ATTR int PARSER_CALL LexerPeekOffset(
    Lexer lexer,
    size_t offset)
{
    
}

PARSER_ATTR inline bool PARSER_CALL LexerIsAtEnd(
    const Lexer* lexer)
{

}

PARSER_ATTR inline void PARSER_CALL LexerTrimWhitespaces(
    const Lexer lexer)
{
    if (!lexer)
        return;

    while (true) {
        int32_t byte = PeekFileBuffer(lexer->file);

        // EOF or error
        if (byte == FILE_BUFFER_EOF) {
            break;
        }

        char c = (char)byte;

        // Not whitespace stop
        if (!lexer->strategy->isWhitespace(c)) {
            break;
        }

        // Update line/column tracking
        if (c == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        else if (c == '\t') {
            lexer->column += 4;  // Tab = 4 spaces (configurable)
        }
        else {
            lexer->column++;
        }

        // Advance
        AdvanceFileBuffer(lexer->file);
    }

}

// ------------------------------------------------------------------------------------------------