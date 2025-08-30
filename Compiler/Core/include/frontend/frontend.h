// -----------------------------------------------------------------------------------------------------------------
//  @file    frontend.h
//  @author  Perijn Huijser
//  @date    2026-08-30
//  @version 1.0
//
//  @brief
//  The public interface for mapping and managing all front-end features of the compiler.
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
#ifndef _CORE_FRONTEND_H_
#define _CORE_FRONTEND_H_

// -----------------------------------------------------------------------------------------------------------------
//  Includes
// -----------------------------------------------------------------------------------------------------------------

/* Standard headers */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Project headers */
#include "status.h"

#include "frontend_pass.h"

// -----------------------------------------------------------------------------------------------------------------
//  Public Types
// -----------------------------------------------------------------------------------------------------------------

typedef struct _frontend_config {
	const char* output_file;
    const char* output_dir;

    fe_pass_plan_t passes;
} frontend_config_t;

typedef struct _frontend_driver frontend_driver_t;
typedef frontend_driver_t* frontend_handle_t;

status_err_t create_frontend(const frontend_config_t* config, frontend_handle_t* fe);
status_err_t destroy_frontend(frontend_handle_t fe);

status_err_t frontend_add_tu();

status_err_t frontend_dispatch();

// -----------------------------------------------------------------------------------------------------------------

#endif // !_CORE_FRONTEND_H_

// -----------------------------------------------------------------------------------------------------------------
