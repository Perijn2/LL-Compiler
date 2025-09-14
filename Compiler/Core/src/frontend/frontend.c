// -----------------------------------------------------------------------------------------------------------------
//  @file    frontend.c
//  @author  Perijn Huijser
//  @date    2026-08-30
//  @version 1.0
//
//  @brief Links the public interface to actual source code. 
//  This source file implements the core functionality of the front-end.
//
//  @details
//  - Defines the necessary types and definitions for the compiler front-end.
//  - Ensures easy access and management for translation units.
//  - Designed to be used by the compiler front-end and driver layer.
//
// -----------------------------------------------------------------------------------------------------------------
//  @changelog
// -----------------------------------------------------------------------------------------------------------------
//  Version 1.0 - 2026-08-30
//  - Initial release
//  - Added definitions for the structures and functions
// -----------------------------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------------------------

/* Standard headers */
#include <stdlib.h>

/* Project headers */
#include "frontend/frontend.h"

// -----------------------------------------------------------------------------------------------------------------
//  Private Functions
// -----------------------------------------------------------------------------------------------------------------

static status_err_t fe_pass_init_noop(struct frontend_ctx_t* fe, void** out_user) {
	if (!fe || !out_user) return STATUS_ERR_INVALID_ARG;

	*out_user = NULL;
	return STATUS_OK;
}

// -----------------------------------------------------------------------------------------------------------------
// Private APIs
// -----------------------------------------------------------------------------------------------------------------

typedef struct _frontend_driver {
	frontend_config_t config;
	
};

status_err_t fe_create_driver(const frontend_config_t* config, frontend_handle_t* fe)
{
	if (!config || !fe) return STATUS_ERR_INVALID_ARG;

	/* Heap allocating the front-end driver */
	frontend_driver_t* driver = (frontend_driver_t*)malloc(sizeof(frontend_driver_t));
	if (!driver) return STATUS_ERR_NON_MEM;

	driver->config = *config;
	*fe = driver;
	return STATUS_OK;
}

status_err_t fe_destroy_driver(frontend_handle_t fe)
{
	if (!fe) return STATUS_ERR_INVALID_ARG;

	free(fe);
	return STATUS_OK;
}