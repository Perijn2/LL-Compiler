//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: preprocessor.h
* Modified: 16/04/2025
*/
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

/*–––– Project headers ––––*/
#include "token.h"              // TokenKind, Token
#include "raw_lexer.h"          // RawLexer
#include "token_provider.h"     // TokenProvider + tp_raw_new/stream_new
#include "utils/file_buf.h"        // FileBuf

//------------------------------------------------------------------------------
//#include "collections/vector.h"  // simple stretchy?buffer or your own
//#include "collections/deque.h"
//#include "macro_table.h"         // mt_lookup / mt_define / mt_undef
//#include "cond_stack.h"          // conditional?compilation bookkeeping

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

typedef enum macro_directives {
    DIRECTIVE_UNKNOWN = 0,
    DIRECTIVE_INCLUDE,
    DIRECTIVE_DEFINE,
    DIRECTIVE_UNDEFINE,
    DIRECTIVE_IF,
    DIRECTIVE_IFDEF,
    DIRECTIVE_IFNDEF,
    DIRECTIVE_ELIF,
    DIRECTIVE_ELSE,
    DIRECTIVE_ENDIF,
    DIRECTIVE_PRAGMA,
} macro_directives_t;

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct _compile_unit compile_unit_t;

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

typedef struct {
    const char* symbol;
    macro_directives_t type;
} directive_def;

typedef struct _preprocessor {
    /* 1. replay queue tokens already decided but not yet emitted */
    //Deque(Token)   replay;

    /* 2. provider stack sources of raw tokens (RAW, STREAM, HEADER?CACHE) */
    //Vector(TokenProvider*) providers;
    token_provider_t* stack;
    uint32_t num_stack;

    /* 3. macro table stores #define bodies */
    //MacroTable     macros;

    /* 4. conditional compilation stack */
    //CondStack      cond;

    /* back pointer for I/O + diagnostics */
    compile_unit_t* unit;
} preprocessor_t;

//------------------------------------------------------------------------------
// Forward declerations
//------------------------------------------------------------------------------

void gen_directives();

void pp_init(preprocessor_t* pp, compile_unit_t* task, token_provider_t* root);
void pp_destroy(preprocessor_t* pp);

token_t* pp_lex(preprocessor_t* pp);

void     push_provider(preprocessor_t* pp, token_provider_t* tp);
void     pop_provider(preprocessor_t* pp);

token_t  *get_raw_token(preprocessor_t* pp);
void     handle_directive(preprocessor_t* pp, token_provider_t* cur);
void     expand_macro(preprocessor_t* pp, const token_t* ident);
