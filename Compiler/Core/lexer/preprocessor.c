//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: preprocessor.h
* Modified: 16/04/2025
*/
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

/*–––– Standard headers ––––*/
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*–––– Project headers ––––*/
#include "compiler/task.h"

#include "preprocessor.h"

#include "utils/khash.h"

//------------------------------------------------------------------------------
// Symbol Table Types
//------------------------------------------------------------------------------

KHASH_MAP_INIT_STR(directives, directive_def*)

khash_t(directives)* directive_tbl;

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

const directive_def pp_directives[] = {
    { "include", DIRECTIVE_INCLUDE  },
    { "define",  DIRECTIVE_DEFINE   },
    { "undef",   DIRECTIVE_UNDEFINE },
    { "if",      DIRECTIVE_IF       },
    { "ifdef",   DIRECTIVE_IFDEF    },
    { "ifndef",  DIRECTIVE_IFNDEF   },
    { "elif",    DIRECTIVE_ELIF     },
    { "else",    DIRECTIVE_ELSE     },
    { "endif",   DIRECTIVE_ENDIF    },
    { "pragma",  DIRECTIVE_PRAGMA   },
    { NULL,      DIRECTIVE_UNKNOWN  }   /* sentinel */
};

//------------------------------------------------------------------------------
// Public API's
//------------------------------------------------------------------------------

void gen_directives()
{
    directive_tbl = kh_init(directives);
    for (const directive_def* dir = pp_directives; dir->symbol; dir++) {
        int32_t ret;
        khiter_t k = kh_put(directives, directive_tbl, dir->symbol, &dir);
        if (ret == -1) return; // Hash table full
        if (ret == 0) { // Already exists 
            return;
        }
    }
}

void pp_init(preprocessor_t* pp, compile_unit_t* unit, token_provider_t* root)
{
    memset(pp, 0, sizeof(preprocessor_t));
    pp->unit = unit;

    ///* containers  use your own stretchy?buffer impls */
    //vec_init(&pp->providers);
    //deq_init(&pp->replay);
    //mt_init(&pp->macros);
    //cs_init(&pp->cond);

    push_provider(pp, root);
}

void pp_destroy(preprocessor_t* pp)
{
    /*while (!vec_empty(&pp->providers)) {
        token_provider_t* tp = vec_back(&pp->providers);
        tp_destroy(tp);
        vec_pop_back(&pp->providers);
    }*/

    /*deq_free(&pp->replay);
    mt_free(&pp->macros);
    cs_free(&pp->cond);*/

    /* no need to free pp  caller owns it */
}

token_t* pp_lex(preprocessor_t* pp)
{
    /* 1. first return anything waiting in replay */
    /*if (!deq_empty(&pp->replay))
        return deq_pop_front(&pp->replay);*/

        /* 2. pull from current provider */

    token_provider_t* cur = pp->stack;
    token_t* tok = cur->next(cur);

    /* provider exhausted ->  pop */
    if (tok->type == TOK_EOF) {
        tp_destroy(cur);
        pop_provider(pp);
        return pp_lex(pp);
    }

    /* consume PP directive */
    if (tok->type == TOK_HASH && tok->bol_hash) {
        handle_directive(pp, cur);
        return pp_lex(pp);
    }

    /* macro expansion */
    //if (tok.type == TOK_IDENT) {
    //    if (MacroInfo* m = mt_lookup(&pp->macros, tok.sp)) {
    //        expand_macro(pp, &tok);
    //        continue;             // expansion filled replay / stream
    //    }
    //}

    return tok;                   // ordinary token ready for parser
    /*token_t eof_tok = { .type = TOK_EOF };
    return &eof_tok;*/
}


void push_provider(preprocessor_t* pp, token_provider_t* tp)
{
    pp->stack->next = tp;
    pp->stack = tp;
    pp->num_stack++;
}

void pop_provider(preprocessor_t* pp)
{
    token_provider_t* old = pp->stack;
    pp->stack = old->next;
    old->next = NULL;
    pp->num_stack--;
}

// Placeholder implementations fill out as you bring features online


/* Parse and execute a directive.  Currently stubs out each branch. */
void handle_directive(preprocessor_t* pp, token_provider_t* cur)
{
    token_t* id = cur->next(cur);           // read identifier after '#'
    if (!id) { return; };

    directive_def* def;
    
    khiter_t k = kh_get(directives, directive_tbl, id->text);
    if (k != kh_end(directive_tbl)) {
        def = kh_value(directive_tbl, k);
    }

    switch (def->type)
    {
    case DIRECTIVE_INCLUDE:
        break;
    case DIRECTIVE_DEFINE:
        break;
    case DIRECTIVE_UNDEFINE:
        break;
    case DIRECTIVE_IF:
        break;
    case DIRECTIVE_PRAGMA:
        break;
    default:
        fprintf(stderr, "%s:%u: unknown preprocessor directive\n",
                    pp->unit ? pp->unit->source_path : "<stdin>", id->line);
        break;
    }

    //if (tok_eq(id, "include")) {
    //    // TODO: open header create FileBuf + RawLexer push provider
    //}
    //else if (tok_eq(id, "define")) {
    //    // TODO: parse macro params & body insert into pp->macros
    //}
    //else if (tok_eq(id, "undef")) {
    //    // TODO: remove from macro table
    //}
    //else if (tok_eq(id, "if") || tok_eq(id, "ifdef") || tok_eq(id, "ifndef")) {
    //    // TODO: conditional?compilation
    //}
    //else if (tok_eq(id, "elif") || tok_eq(id, "else") || tok_eq(id, "endif")) {
    //    // TODO: conditional?compilation state updates
    //}
    //else if (tok_eq(id, "pragma")) {
    //    // TODO: parse ignore pragma
    //}
    //else {
    //    fprintf(stderr, "%s:%u:%u: unknown preprocessor directive\n",
    //        pp->task ? pp->task->current_file : "<stdin>", id.line, id.col);
    //    // skip to end of line
    //}
}

/* Expand object?like or function?like macro.  Stubbed for now. */
void expand_macro(preprocessor_t* pp, const token_t* ident)
{
    /*MacroInfo* m = mt_lookup(&pp->macros, ident->sp);
    if (!m) return;*/

    // TODO: collect arguments if function?like
    // TODO: substitute formals ? actuals
    // TODO: either push STREAM provider or enqueue into replay deque
}


// compile?time sanity check (optional) – ensures struct layout assumptions

//_Static_assert(sizeof(token_provider_t) % alignof(max_align_t) == 0,
//    "TokenProvider must be max?align sized");
