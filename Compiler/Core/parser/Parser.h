//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: compile_unit.h
* Moodified: 18/03/2025
*/
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#define FUNC_MODIFIERS (MOD_STATIC | MOD_INLINE)
#define VAR_MODIFIERS  (MOD_CONST | MOD_STATIC | MOD_SIGNED | MOD_UNSIGNED)

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdint.h>

#include "ast_node.h"

#include "lexer/token_provider.h"

#include "utils/arena_alloc.h"

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

typedef enum {
    AST_STATUS_OK = 0,
} ast_status_t;

//------------------------------------------------------------------------------
// Hash maps
//------------------------------------------------------------------------------

KHASH_MAP_INIT_STR(symbols, ast_node_t*)  // Identifier  AST node
KHASH_MAP_INIT_STR(types, ast_node_t*)    // Type name   AST node

//------------------------------------------------------------------------------
// Scope and Parser Context
//------------------------------------------------------------------------------

typedef struct ast_scope {
    size_t varCount;
    struct ast_scope* parent;
    khash_t(symbols)* symTable;
} ast_scope_t;

typedef struct ast_parser {
    struct compile_unit* unit;
    // Token stream
    //Token** tokens;
    size_t pos;
    size_t size;
   // Token* current;

    /* token source */
    token_provider_t* tp;
    token_t* current_tok;          /* 1-token lookahead */

    // Symbol/type tables
    khash_t(symbols)* funcTable;
    khash_t(types)* typeTable;

    /* scopes */
    ast_scope_t* global_scope;
    ast_scope_t* scope;         /* current */

    /* modifiers/state collected while parsing decl-specifiers */
    type_flags_t modifiers;     /* your flags enum */

    /* memory & diagnostics */
    Arena* arena;
    //diagnostics_t* diags;

    // TODO: Error reporting
    // ErrorList* errors;
    // const char* source;
} ast_parser_t;

//------------------------------------------------------------------------------
// Parser Interface
//------------------------------------------------------------------------------

// Lifecycle
static ast_status_t create_parser(compile_unit_t* unit, Arena* arena, ast_parser_t* parser);
static ast_status_t destroy_parser(ast_parser_t* parser);

// Scope management
static ast_status_t enter_scope(ast_parser_t* parser);
static ast_status_t exit_scope(ast_parser_t* parser);

// Symbol management
//static bool addSymbol(ast_parser_t* parser, const char* name, ASTNode* node);
//bool addCustomType(Parser* parser, const char* name, ASTNode* node);
//bool addFunctionDecleration(Parser* parser, const char* name, ASTNode* node);

//ASTNode* lookupSymbol(Parser* parser, const char* name);
//ASTNode* lookupFunction(Parser* parser, const char* name);

// Token handling
static token_t* advance_token(ast_parser_t* parser, uint8_t offset);
static token_t* expect_token(ast_parser_t* parser, token_type_t type, const char* errorMessage);
static token_t* peek_token(ast_parser_t* parser, uint8_t offset);

// Modifier handling
static ast_status_t push_modifier(ast_parser_t* parser, type_flags_t flag);
static ast_status_t apply_modifiers(ast_parser_t* parser);
//uint8_t validateModifiers(Parser* parser, ASTType ctx);

/* Parsing entry points ----------------------------------------------------- */
ast_status_t  parser_parse_unit(ast_parser_t* p, ast_node_t** out_root);

/* Statements / declarations / expressions (return AST nodes) */
ast_node_t* parse_declaration(ast_parser_t* p);
ast_node_t* parse_statement(ast_parser_t* p);
ast_node_t* parse_block(ast_parser_t* p);                  /* { ... } */
//static ASTNode* parseFunctionCall(ast_parser_t* parser);

ast_node_t* parse_function_definition(ast_parser_t* p);    /* proto + body   */
ast_node_t* parse_function_prototype(ast_parser_t* p);     /* proto only     */
ast_node_t* parse_parameter_list(ast_parser_t* p);
ast_node_t* parse_argument_list(ast_parser_t* p);

ast_node_t* parse_if_statement(ast_parser_t* parser);
ast_node_t* parse_while_statement(ast_parser_t* parser);
ast_node_t* parse_for_statement(ast_parser_t* parser);
ast_node_t* parse_return_statement(ast_parser_t* parser);

ast_node_t* parse_expression(ast_parser_t* p, int min_prec);
ast_node_t* parse_primary(ast_parser_t* p);
ast_node_t* parse_unary(ast_parser_t* p);

/* Type parsing */
type_info_t* parse_type_name(ast_parser_t* p);

//ASTNode* parseFunctionProto(Parser* parser);

uint8_t getPrecedence(Operators op);
uint8_t isDatatype(KeywordType type);
uint8_t isLiteral(TokenType type);
uint8_t isUnary(Operators type);
ASTNode* newNode(Parser* parser, ASTType type);

#endif // PARSER_H
