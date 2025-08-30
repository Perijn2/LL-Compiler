// -----------------------------------------------------------------------------------------------------------------
// Name: Status.h
// 
// Author: Perijn Huijser
// Date: 2026-08-30
// Version: 1.0
// 
// Description: This file contains the status codes used throughout the compiler.
// This way we have a unified way of reporting errors and statuses.
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
#ifndef _STATUS_H_
#define _STATUS_H_

/* Helper macro for giving the byte value of a specific bit 'n' */
#define BIT(n) (1U << (n))

// -----------------------------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------------------------

#include <stdint.h>

// -----------------------------------------------------------------------------------------------------------------
// Status Codes
// -----------------------------------------------------------------------------------------------------------------

/* Status error type */
typedef uint32_t status_err_t;

// -----------------------------------------------------------------------------------------------------------------
// Status Codes
// -----------------------------------------------------------------------------------------------------------------

#define STATUS_OK 0x0000 // No error, operation successful

#define ERROR_BASE_CODE 0x1000 // Base code for all errors
#define STATUS_ERR_FAIL			 (ERROR_BASE_CODE + 0x01) // General failure
#define STATUS_ERR_INVALID_ARG	 (ERROR_BASE_CODE + 0x02) // Invalid argument provided
#define STATUS_ERR_NON_MEM         (ERROR_BASE_CODE + 0x03) // Memory allocation failure
#define STATUS_ERR_FILE_NOT_FOUND  (ERROR_BASE_CODE + 0x04) // Source or object file not found
#define STATUS_ERR_FILE_IO         (ERROR_BASE_CODE + 0x05) // File I/O error
#define STATUS_ERR_SYNTAX          (ERROR_BASE_CODE + 0x06) // Syntax error in source
#define STATUS_ERR_SEMANTIC        (ERROR_BASE_CODE + 0x07) // Semantic error in source
#define STATUS_ERR_LINK            (ERROR_BASE_CODE + 0x08) // Linking error
#define STATUS_ERR_UNSUPPORTED     (ERROR_BASE_CODE + 0x09) // Unsupported feature or option
#define STATUS_ERR_CONFIG          (ERROR_BASE_CODE + 0x0A) // Invalid or missing configuration
#define STATUS_ERR_TIMEOUT         (ERROR_BASE_CODE + 0x0B) // Operation timed out
#define STATUS_ERR_PERMISSION      (ERROR_BASE_CODE + 0x0C) // Permission denied
#define STATUS_ERR_INTERNAL        (ERROR_BASE_CODE + 0x0D) // Internal compiler error

// -----------------------------------------------------------------------------------------------------------------

#endif // !_STATUS_CODES_H_

// -----------------------------------------------------------------------------------------------------------------