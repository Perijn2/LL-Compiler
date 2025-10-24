// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "ParserCore.h"
#include "Results.h"

#include "lexer/Lexer.h"
#include "lexer/Token.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

PARSER_CORE_DEFINE_HANDLE(ASTParser)
PARSER_CORE_DEFINE_HANDLE(ASTScope)
PARSER_CORE_DEFINE_HANDLE(ASTSymbol)      // for symbol table entries
PARSER_CORE_DEFINE_HANDLE(ASTType)        // for type representation
PARSER_CORE_DEFINE_HANDLE(ASTNode)

typedef enum ParserASTNodeType {
    AST_NODE_TYPE_NONE = 0,

    // ===== Declarations =====
    AST_NODE_TYPE_TRANSLATION_UNIT,      // root of the AST (entire file)
    AST_NODE_TYPE_FUNCTION_DECL,         // function declaration/definition
    AST_NODE_TYPE_VARIABLE_DECL,         // variable declaration
    AST_NODE_TYPE_PARAMETER_DECL,        // function parameter
    AST_NODE_TYPE_STRUCT_DECL,           // struct declaration
    AST_NODE_TYPE_UNION_DECL,            // union declaration
    AST_NODE_TYPE_ENUM_DECL,             // enum declaration
    AST_NODE_TYPE_TYPEDEF_DECL,          // typedef declaration

    // ===== Type Specifiers =====
    AST_NODE_TYPE_TYPE_SPECIFIER,        // int, float, char, etc.
    AST_NODE_TYPE_POINTER_TYPE,          // pointer type (int*)
    AST_NODE_TYPE_ARRAY_TYPE,            // array type (int[10])
    AST_NODE_TYPE_FUNCTION_TYPE,         // function type signature

    // ===== Statements =====
    AST_NODE_TYPE_COMPOUND_STMT,         // block/scope { ... }
    AST_NODE_TYPE_EXPRESSION_STMT,       // expression statement (x = 5;)
    AST_NODE_TYPE_RETURN_STMT,           // return statement
    AST_NODE_TYPE_IF_STMT,               // if statement
    AST_NODE_TYPE_WHILE_STMT,            // while loop
    AST_NODE_TYPE_DO_WHILE_STMT,         // do-while loop
    AST_NODE_TYPE_FOR_STMT,              // for loop
    AST_NODE_TYPE_SWITCH_STMT,           // switch statement
    AST_NODE_TYPE_CASE_STMT,             // case label
    AST_NODE_TYPE_DEFAULT_STMT,          // default label
    AST_NODE_TYPE_BREAK_STMT,            // break statement
    AST_NODE_TYPE_CONTINUE_STMT,         // continue statement
    AST_NODE_TYPE_GOTO_STMT,             // goto statement
    AST_NODE_TYPE_LABEL_STMT,            // label for goto

    // ===== Expressions =====
    AST_NODE_TYPE_BINARY_EXPR,           // binary operations (a + b, a * b, etc.)
    AST_NODE_TYPE_UNARY_EXPR,            // unary operations (++a, -x, *ptr, &var)
    AST_NODE_TYPE_TERNARY_EXPR,          // ternary conditional (a ? b : c)
    AST_NODE_TYPE_CALL_EXPR,             // function call
    AST_NODE_TYPE_CAST_EXPR,             // type cast
    AST_NODE_TYPE_ASSIGNMENT_EXPR,       // assignment (=, +=, -=, etc.)
    AST_NODE_TYPE_MEMBER_EXPR,           // struct/union member access (obj.member)
    AST_NODE_TYPE_ARROW_EXPR,            // pointer member access (ptr->member)
    AST_NODE_TYPE_ARRAY_SUBSCRIPT_EXPR,  // array subscript (arr[i])
    AST_NODE_TYPE_SIZEOF_EXPR,           // sizeof operator
    AST_NODE_TYPE_COMMA_EXPR,            // comma operator (a, b, c)

    // ===== Literals & Identifiers =====
    AST_NODE_TYPE_INTEGER_LITERAL,       // integer constant
    AST_NODE_TYPE_FLOAT_LITERAL,         // floating-point constant
    AST_NODE_TYPE_STRING_LITERAL,        // string literal
    AST_NODE_TYPE_CHAR_LITERAL,          // character literal
    AST_NODE_TYPE_IDENTIFIER,            // identifier reference

    // ===== Initialization =====
    AST_NODE_TYPE_INITIALIZER,           // variable initializer
    AST_NODE_TYPE_INITIALIZER_LIST,      // initializer list { a, b, c }

    // ===== Preprocessor (optional, if tracking) =====
    AST_NODE_TYPE_MACRO_EXPANSION,       // macro expansion node

    AST_NODE_TYPE_COUNT                  // total count of node types
} ParserASTNodeType;


// ===== Language-Specific Parsing Callbacks =====

/**
 * @brief Callback to parse a primary expression (literals, identifiers, etc.)
 * 
 * @param parser The parser context
 * 
 * @return AST node representing the primary expression, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParsePrimaryExpression)(ASTParser parser, ASTNode node);

/**
 * @brief Callback to parse a declaration
 * 
 * @param parser The parser context
 * 
 * @return AST node representing the declaration, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParseDeclaration)(ASTParser parser, ASTNode node);

/**
 * @brief Callback to parse a statement
 * 
 * @param parser The parser context
 * 
 * @return AST node representing the statement, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParseStatement)(ASTParser parser, ASTNode node);

/**
 * @brief Callback to parse a type specifier
 * 
 * @param parser The parser context
 * 
 * @return AST node representing the type, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParseTypeSpecifier)(ASTParser parser, ASTNode node);

/**
 * @brief Callback to check if current token is a valid type name
 * 
 * @param parser The parser context
 * 
 * @return true if current token is a type name, false otherwise
 */
typedef bool (PARSER_PTR* PFN_IsTypeName)(ASTParser parser);

/**
 * @brief Callback to get operator precedence for a given token
 * 
 * @param token The token to check
 * 
 * @return Precedence level (higher = tighter binding)
 */
typedef uint16_t (PARSER_PTR* PFN_GetOperatorPrecedence)(LexerToken token);

/**
 * @brief Callback to parse a binary operator expression
 * 
 * @param parser[in] The parser context
 * @param left[in] Left-hand side expression
 * @param precedence[in] Minimum precedence level
 * @param AST[out] node representing the binary expression, or NULL on error
 * 
 * @return ParserResult representing the status code of the operation
 */
typedef ParserResult (PARSER_PTR* PFN_ParseBinaryOperator)(
    ASTParser parser,
    ASTNode left,
    int precedence,
    ASTNode node);

/**
 * @brief Callback to parse a unary operator expression
 * 
 * @param parser The parser context
 * 
 * @return AST node representing the unary expression, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParseUnaryOperator)(ASTParser parser);

/**
 * @brief Callback to parse function parameters
 * 
 * @param parser The parser context
 * 
 * @return AST node representing parameter list, or NULL on error
 */
typedef ParserResult (PARSER_PTR* PFN_ParseParameterList)(ASTParser parser);

/**
 * @brief Language-specific parsing strategy
 *
 * @description Contains function pointers for all language-specific parsing
 *              operations. Different languages (C, C++, custom DSL).
 */
typedef struct ParserLanguageStrategy {
    const char* languageName;                         // e.g., "C89", "C99", "C++11"

    // Expression parsing
    PFN_ParsePrimaryExpression    parsePrimaryExpr;
    PFN_ParseBinaryOperator       parseBinaryOp;
    PFN_ParseUnaryOperator        parseUnaryOp;
    PFN_GetOperatorPrecedence     getOpPrecedence;

    // Declaration parsing
    PFN_ParseDeclaration          parseDeclaration;
    PFN_ParseTypeSpecifier        parseTypeSpec;
    PFN_IsTypeName                isTypeName;
    PFN_ParseParameterList        parseParamList;

    // Statement parsing
    PFN_ParseStatement            parseStatement;

    // Optional: language-specific features
    void* userData;                                   // for language-specific state
} ParserLanguageStrategy;

// Parser initialization
PARSER_ATTR void PARSER_CALL ASTParser_Init(
    ASTParser parser,
    Lexer lexer);

// Main parsing entry point
PARSER_ATTR ASTNode PARSER_CALL ASTParser_Parse(
    ASTParser parser);

// Utility functions
PARSER_ATTR void PARSER_CALL ASTParser_Advance(
    ASTParser* parser);
PARSER_ATTR bool PARSER_CALL ASTParser_Expect(
    ASTParser* parser,
    TokenTypeFlags expectedType);
PARSER_ATTR bool PARSER_CALL ASTParser_Match(
    ASTParser* parser,
    TokenTypeFlags type);

PARSER_ATTR ParserResult PARSER_CALL ASTParserCurrentToken(
    ASTParser parser,
    LexerToken token);

// Parse declarations
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseDeclaration(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseFunctionDeclaration(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseVariableDeclaration(
    ASTParser* parser);

// Parse statements
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseStatement(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseCompoundStatement(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseIfStatement(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseWhileStatement(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseForStatement(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseReturnStatement(
    ASTParser* parser);

// Parse expressions (by precedence)
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseExpression(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseAssignmentExpression(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseConditionalExpression(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseBinaryExpression(
    ASTParser* parser,
    int precedence);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParseUnaryExpression(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParsePrimaryExpression(
    ASTParser* parser);
PARSER_ATTR ASTNode* PARSER_CALL ASTParser_ParsePostfixExpression(
    ASTParser* parser);

// ------------------------------------------------------------------------------------------------
#endif // !
// ------------------------------------------------------------------------------------------------