//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: token_provider.h
* Moodified: 18/03/2025
*/
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdlib.h>


/*–––– Project headers ––––*/
#include "raw_lexer.h" 

//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------

typedef struct token_provider_t token_provider_t;

//------------------------------------------------------------------------------
// structs
//------------------------------------------------------------------------------

struct token_provider_t {
	token_t *(*next)(token_provider_t* self);
	void    (*destroy)(token_provider_t* self);
	void*	state;
	struct token_provider_t* next_prv;
};

//------------------------------------------------------------------------------
// Public API's
//------------------------------------------------------------------------------

/* RAW provider: takes ownership of *rl  (caller allocs, provider frees). */
token_provider_t* tp_raw_new(raw_lexer_t* rl);

/* STREAM provider: non-owning view of an existing token vector. */
token_provider_t* tp_stream_new(const token_t* slice, size_t n);

/* Destroy (calls provider->destroy and frees the wrapper). */
void tp_destroy(token_provider_t* tp);
