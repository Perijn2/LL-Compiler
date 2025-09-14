//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: token_provider.h
* Moodified: 18/03/2025
*/
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <string.h>  // memset

#include "token_provider.h"

//------------------------------------------------------------------------------
// Public API's
//------------------------------------------------------------------------------

// STREAM provider

typedef struct {
    const token_t*  tok;
    size_t          len, idx;
} stream_st;

static token_t* stream_next(token_provider_t* self)
{
    stream_st* st = (stream_st*)self->state;

    if (st->idx < st->len)
        return &st->tok[st->idx++];

    static token_t eof_tok = { .type = TOK_EOF };
    return &eof_tok;
}

static void stream_destroy(token_provider_t* self)
{
    free(self->state);          /* only the state was heap allocd */
    free(self);
}

token_provider_t* tp_stream_new(const token_t* slice, size_t n)
{
    token_provider_t* tp = malloc(sizeof * tp);
    stream_st* st = malloc(sizeof * st);
    st->tok = slice;
    st->len = n;
    st->idx = 0;

    tp->next = stream_next;
    tp->destroy = stream_destroy;
    tp->state = st;
    return tp;
}

// RAW provider

typedef struct {
    raw_lexer_t rl;                
} raw_st;

static token_t *raw_next(token_provider_t* self)
{
    raw_st* st = (raw_st*)self->state;
    token_t* tk;
    if (rl_next(&st->rl, tk) != LEX_STATUS_OK)
    {
        // Report error
        return NULL;
    }
    return tk;
}
static void raw_destroy(token_provider_t* self)
{
    raw_st* st = (raw_st*)self->state;
    rl_destroy(&st->rl);
    free(self);                 /* state is inside self                 */
}

token_provider_t* tp_raw_new(raw_lexer_t* rl_external)
{
    token_provider_t* tp = malloc(sizeof(token_provider_t) + sizeof(raw_st));
    raw_st* st = (raw_st*)(tp + 1);      /* state right after tp */

    memcpy(&st->rl, rl_external, sizeof(raw_lexer_t)); /* shallow copy  */
    tp->next = raw_next;
    tp->destroy = raw_destroy;
    tp->state = st;

    /* callers RawLexer is now *moved*; zero it to prevent double-free   */
    memset(rl_external, 0, sizeof(raw_lexer_t));
    return tp;
}

void tp_destroy(token_provider_t* tp)
{
    if (tp) tp->destroy(tp);
}