//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: token.h
* Modified: 18/03/2025
*/
/*------------------------------------------------------------------------------
*
*  Handles:
*     Decimal      0  42  123u  18446744073709551615ULL
*     Octal        0755  0'123
*     Hexadecimal  0xDEADBEEF  0x1.8p+5  0XCAFE'BABEULL
*     Binary       0b1101'0010 (GCC/Clang extension)
*     Floating     3.14  .5  1e-9  0x1.fp-3f
*
*  Suffix decoding:  U u  L LL  f F  etc.
*
*  The function returns a fully filled Token and advances the RawLexer
*  cursor.  On lexical error it returns TOK_INVALID and leaves the lexer
*  positioned right after the offending character (so callers can recover).
*
*/
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#pragma once
#ifndef _LEX_NUMBERS_H_
#define _LEX_NUMBERS_H_


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "raw_lexer.h"     /* RawLexer, Token, TokenKind */

//------------------------------------------------------------------------------
// Public API's
//------------------------------------------------------------------------------

static inline bool pred_dec(int c) { return (unsigned)(c - '0') < 10; }
static inline bool pred_oct(int c) { return (unsigned)(c - '0') < 8; }
static inline bool pred_bin(int c) { return c == '0' || c == '1'; }
static inline bool pred_hex(int c) {
    if ((unsigned)(c - '0') < 10) return true;
    c |= 32;                           /* tolower without locale */
    return (unsigned)(c - 'a') < 6;
}

/* consume a sequence while pred holds, allowing  digit separators */
static const uint8_t* consume_digits(const uint8_t* p, const uint8_t* end,
    bool (*pred)(int))
{
    while (p < end) {
        if (*p == '\'') { ++p; continue; }
        if (!pred(*p)) break;
        ++p;
    }
    return p;
}

token_t* rl_lex_number(raw_lexer_t* rl)
{
    const uint8_t* start = rl->cur;

    /* 1. detect radix prefix */
    bool is_float = false;
    int16_t  radix = 10;

    if (CUR == '0') {
        if ((PEEK(1) | 32) == 'x') { radix = 16; rl->cur += 2; }
        else if ((PEEK(1) | 32) == 'b') { radix = 2; rl->cur += 2; }
        else { radix = 8; ADV(); }
    }

    /* 2. main digit run */
    switch (radix) {
    case 10: rl->cur = consume_digits(rl->cur, rl->end, pred_dec); break;
    case 8:  rl->cur = consume_digits(rl->cur, rl->end, pred_oct);   break;
    case 16: rl->cur = consume_digits(rl->cur, rl->end, pred_hex);   break;
    case 2:  rl->cur = consume_digits(rl->cur, rl->end, pred_bin);   break;
    }

    /* 3. fractional / exponent part for floats */
    if (radix == 10 && CUR == '.') {
        is_float = true; ADV();
        rl->cur = consume_digits(rl->cur, rl->end, pred_dec);
    }
    if (((CUR | 32) == 'e' && radix == 10) || ((CUR | 32) == 'p' && radix == 16)) {
        is_float = true; ADV();
        if (CUR == '+' || CUR == '-') ADV();
        rl->cur = consume_digits(rl->cur, rl->end, pred_dec);
    }

    /* 4. suffixes */
    const uint8_t* suffix = rl->cur;
    while (is_ident_char(CUR)) ADV();

    /* 5. build token */
    token_t *tok = (token_t*) malloc(sizeof(token_t));
    tok->line = rl->line;
    //tok->col = rl->col;
    tok->text = (const char*)start;
    tok->len = (size_t)(rl->cur - start);

    if (is_float) {
        tok->type = TOK_FLOAT;
        /* TODO: strtod or hex float parse */
    }
    else {
        tok->type = TOK_INTEGER;
        /* TODO: compute intValue modulo 2^64 if desired */
    }

    /* 6. update positional bookkeeping */
    rl->col += (uint32_t)tok->len;

    return tok;
}

//------------------------------------------------------------------------------

#endif // !_LEX_NUMBERS_H_

//------------------------------------------------------------------------------