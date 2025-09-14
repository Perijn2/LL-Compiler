// -----------------------------------------------------------------------------------------------------------------
// Name: Translation_unit.h
// 
// Author: Perijn Huijser
// Date: 2026-08-30
// Version: 1.0
// 
// Description: This file contains the translation unit definitions for the compiler driver.
// It is used for a single source file and its associated headers. Each translation unit
// is compiled independently and can be emitted as an object file. Later, these object files
// can be linked together to create the final executable or library.
// 
// -----------------------------------------------------------------------------------------------------------------
// Changelog
// -----------------------------------------------------------------------------------------------------------------
// 
// Version 1.0 - 2026-08-30
// - Initial release
// - Added core definitions and included necessary headers
// - Set up include guard
// 
// 
// -----------------------------------------------------------------------------------------------------------------
// Include Guard
// -----------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef _TRANSLATION_UNIT_H_
#define _TRANSLATION_UNIT_H_

// -----------------------------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------------------------

/* Standard headers */
#include <stdint.h>

/* Common headers */
#include "status.h"
#include "include_map.h"

// -----------------------------------------------------------------------------------------------------------------
// Public definitions
// -----------------------------------------------------------------------------------------------------------------

typedef struct _translation_unit_config {
	/* Source file to compile */
	const char* source_file;
	
	/* Provided include paths*/
	include_search_path_t include_paths;

} translation_unit_config_t;

typedef struct _translation_unit translation_unit_t;
typedef _translation_unit* translation_unit_handle_t;

status_err_t create_translation_unit(const translation_unit_config_t* config, translation_unit_handle_t* tu);

// -----------------------------------------------------------------------------------------------------------------

#endif // !_TRANSLATION_UNIT_H_

// -----------------------------------------------------------------------------------------------------------------
