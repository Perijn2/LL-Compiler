// -----------------------------------------------------------------------------------------------------------------
// Name: Compiler.h
// 
// Author: Perijn Huijser
// Date: 2026-08-30
// Version: 1.0
// 
// Description: This file contains the core definitions of the compiler driver.
// It includes the necessary headers for the user to interface with the compiler.
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
#ifndef _COMPILER_H_
#define _COMPILER_H_

// -----------------------------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------------------------

/* Standard headers */
#include <stdint.h>

/* Common headers */
#include "status.h"

// -----------------------------------------------------------------------------------------------------------------
// Public definitions
// -----------------------------------------------------------------------------------------------------------------

typedef struct _compiler_config{
	const char* name;

} compiler_config_t;

typedef void* compiler_handle_t;

/**
* @brief Initializes the compiler with the given configuration.
* 
* @param[in] config		Pointer to the compiler configuration structure.
* @param[out] handle	Pointer to the compiler handle that will be initialized.
* 
* @return status_err_t	
*			STATUS_OK - on success
*/
status_err_t compiler_init(const compiler_config_t* config, const compiler_handle_t* handle);

// -----------------------------------------------------------------------------------------------------------------

#endif // !_COMPILER_H_

// -----------------------------------------------------------------------------------------------------------------
