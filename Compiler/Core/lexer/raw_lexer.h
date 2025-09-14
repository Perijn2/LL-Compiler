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
#ifndef _RAW_LEXER_H_
#define _RAW_LEXER_H_

#define CUR  (*rl->cur)
#define PEEK(o)  (rl->cur + (o) < rl->end ? rl->cur[(o)] : '\0')
#define ADV()    (++rl->cur)
#define ADV_N(n)   ( rl->cur += (n), rl->col += (uint32_t)(n) )
#define AT_END     (rl->cur >= rl->end)

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

/*–––– Standard headers ––––*/
#include <stddef.h>            /* size_t                                      */
#include <stdint.h>            /* uint8_t, uint32_t                           */
#include <stdbool.h>

/*–––– Project headers ––––*/
#include "token.h"             /* TokenKind, Token                           */
#include "utils/file_buf.h"       /* FileBuf                                    */

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

typedef enum lexer_status {

	LEX_STATUS_OK = 0,
	LEX_STATUS_EOF,

	LEX_STATUS_INVALID_CHAR = 100,
	LEX_STATUS_INVALID_UTF8,
	LEX_STATUS_UNCLOSED_STRING,
	LEX_STATUS_INVALID_ESCAPE,
	LEX_STATUS_UNTERMINATED_COMMENT,
	LEX_STATUS_NUM_OVERFLOW,

	LEX_STATUS_IO_ERROR = 200,
	LEX_STATUS_OUT_OF_MEMORY,
	LEX_STATUS_INIT_EROR,

	LEX_STATUS_INTERNAL_ERROR = 900
} lexer_status_t;

//------------------------------------------------------------------------------
// structs
//------------------------------------------------------------------------------

typedef struct {
    const char* text;
    operator_subtype_t type;     /* whatever you called it */
    uint8_t len;
    uint32_t hash;            /* precomputed for speed */
} operater_def;

typedef struct raw_lexer
{
    /* Raw text buffer (owned by FileBuf) */
    const uint8_t* begin;       /* first byte of file                          */
    const uint8_t* cur;         /* current cursor                              */
    const uint8_t* end;         /* one-past-last byte                          */
    const char* filename;       /* borrowed pointer (for diagnostics)          */
    file_buf_handle* fb;        /* pointer so we can fb_close() in destroy     */

    /* Position bookkeeping --------------------------------------------------*/
    uint32_t line;             
    uint32_t col;             

    bool bol_hash;
} raw_lexer_t;

//------------------------------------------------------------------------------
// Public API
//------------------------------------------------------------------------------


static uint32_t rl_init(raw_lexer_t* rl, const char* filename, const uint8_t* begin, size_t len);
static uint32_t rl_from_file(raw_lexer_t* rl, const char* path);

static void rl_destroy(raw_lexer_t* rl);

static inline token_t* make_tok(token_type_t type, operator_subtype_t sub,
    raw_lexer_t* rl,
    const uint8_t* start,
    uint32_t start_col);

static lexer_status_t rl_next(raw_lexer_t* rl, token_t* tk);

token_t* rl_lex_identifier(raw_lexer_t* rl);
token_t* rl_lex_operator(raw_lexer_t* rl);
token_t* rl_lex_string(raw_lexer_t* rl);

static inline bool rl_is_number_start(const raw_lexer_t* rl);

static inline bool is_indent_start(unsigned char c);
static inline bool is_ident_char(char c);

static inline token_type_t get_punct_kind(unsigned char c);

static size_t rl_trim_comments(char* src, size_t len, char*);
static void rl_trim_whitespaces(raw_lexer_t* lex);


uint64_t octalToInteger(const char* src);
uint64_t binaryToInteger(const char* src);
uint64_t hexadecimalToInteger(const char* src);
uint64_t decimalToInteger(const char* src);

uint64_t octalToFloat(const char* src);
uint64_t binaryToFloat(const char* src);
uint64_t hexadecimalToFloat(const char* src);
uint64_t decimalToFloat(const char* src);

static uint64_t parse_uint64_mod(const uint8_t* p,
    const uint8_t* end,
    int radix)
{
    uint64_t value = 0;
    while (p < end) {
        int d;
        if (*p == '\'') { ++p; continue; }      /* skip digit separator */
        d = (radix == 16) ? hex_value(*p)
            : (radix == 10) ? (*p - '0')
            : (radix == 8) ? (*p - '0')
            : (*p - '0');                       /* radix 2 uses same test */
        if (d < 0 || d >= radix) break;

        /* (value * radix + d) mod 2??  */
        value = value * (uint64_t)radix + (uint64_t)d;
        ++p;
    }
    return value;
}

//------------------------------------------------------------------------------

#endif // !_RAW_LEXER_H_

//------------------------------------------------------------------------------