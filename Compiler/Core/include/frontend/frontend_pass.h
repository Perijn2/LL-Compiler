// -----------------------------------------------------------------------------------------------------------------
//  @file    frontend_pass.h
//  @author  Perijn Huijser
//  @date    2026-08-30
//  @version 1.0
//
//  @brief
//  Provides the necessary definitions for controlling and customizing the front-end
//  complilation passes.
//
//  @details
//  - Provides the necessary types and definitions for the compiler front-end.
//  - Ensures easy access and management for translation units.
//  - Designed to be used by the compiler front-end and driver layer.
//
// -----------------------------------------------------------------------------------------------------------------
//  @changelog
// -----------------------------------------------------------------------------------------------------------------
//  Version 1.0 - 2026-08-30
//  - Initial release
//  - Added core definitions and include guard
// -----------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef FRONTEND_PASS_H_
#define FRONTEND_PASS_H_

// -----------------------------------------------------------------------------------------------------------------
//  Includes
// -----------------------------------------------------------------------------------------------------------------

/* Standard headers */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Project headers */
#include "status.h"

// -----------------------------------------------------------------------------------------------------------------
//  Public Types
// -----------------------------------------------------------------------------------------------------------------

/* Stable pass IDs (ordinal order); add new ones at the end. */
typedef enum {
    FE_PASSID_PREPROCESS = 0,
    FE_PASSID_PARSE = 1,
    FE_PASSID_SEMANTIC = 2,
    FE_PASSID_IR_GEN = 3,
    FE_PASSID_COUNT
} fe_pass_id_t;

/* Pass flags derived from IDs; NONE must be zero. */
typedef uint32_t fe_pass_mask_t;
enum {
    FE_PASS_NONE = 0u,
    FE_PASS_PREPROCESS = BIT(FE_PASSID_PREPROCESS),
    FE_PASS_PARSE = BIT(FE_PASSID_PARSE),
    FE_PASS_SEMANTIC = BIT(FE_PASSID_SEMANTIC),
    FE_PASS_IR_GEN = BIT(FE_PASSID_IR_GEN),
    FE_PASS_ALL = (FE_PASS_PREPROCESS | FE_PASS_PARSE |
        FE_PASS_SEMANTIC | FE_PASS_IR_GEN)
};

/* Function pointer types */
typedef status_err_t(*fe_pass_init_fn)(struct frontend_ctx_t* fe, void** out_user);
typedef status_err_t(*fe_pass_run_fn)(struct frontend_ctx_t* fe, const void* cfg, void* ctx);
typedef void         (*fe_pass_dispose_fn)(struct frontend_ctx_t* fe, void* user);

typedef void* (*fe_pass_alloc_fn)(size_t size, void* ctx);
typedef void  (*fe_pass_free_fn)(void* ptr, void* ctx);

/* Pass descriptor (static, typically registered at startup) */
typedef struct {
    const char*  name;       /* "preprocess", "parse", ... */
    const char*  doc;        /* brief description (optional) */
    fe_pass_id_t id;         /* stable ID */
    uint32_t flags;      /* reserved for future use */
    fe_pass_mask_t deps;       /* passes that must run before this one */
	uint32_t debug_level; /* verbosity level for debug logging */

    fe_pass_init_fn    init;       /* optional; can be NULL */
    fe_pass_run_fn     run;        /* required */
    fe_pass_dispose_fn dispose;    /* optional; can be NULL */

	fe_pass_alloc_fn malloc_fn; /* optional; can be NULL */
	fe_pass_free_fn  free_fn;   /* optional; can be NULL */
} fe_pass_desc_t;

/* Execution plan lets you select passes and attach per-pass configs */
typedef struct {
    fe_pass_mask_t mask;                 /* which passes to run */
    const void* cfg[FE_PASSID_COUNT]; /* cfg[id] -> pass-specific config or NULL */
} fe_pass_plan_t;

// -----------------------------------------------------------------------------------------------------------------

#endif // !FRONTEND_PASS_H_

// -----------------------------------------------------------------------------------------------------------------
