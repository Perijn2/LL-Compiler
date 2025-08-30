// -----------------------------------------------------------------------------------------------------------------
//  @file    include_map.h
//  @author  Perijn Huijser
//  @date    2026-08-30
//  @version 1.0
//
//  @brief
//  Utility types and definitions for managing include directories and their search semantics.
//  Provides enums and containers to represent include path kinds, syntax forms, and ordered
//  vectors of search directories used in translation units.
//
//  @details
//  - Automates the collection and organization of include paths.
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
#ifndef _INCLUDE_MAP_H
#define _INCLUDE_MAP_H

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

/**
 * @enum include_type_t
 * @brief Classification of include directory search type.
 */
typedef enum {
    INCLUDE_TYPE_NONE = 0,  /* Not an include directory */
    INCLUDE_TYPE_USER,      /* Directories added with `-I` */
    INCLUDE_TYPE_SYSTEM     /* Directories added with `-isystem` */
} include_type_t;

/**
 * @enum include_syntax_t
 * @brief Syntax form of the include directive.
 */
typedef enum {
    INCLUDE_SYNTAX_QUOTED, /* `#include "foo.h"` — current dir is searched first */
    INCLUDE_SYNTAX_ANGLED  /* `#include <foo.h>` — current dir is skipped */
} include_syntax_t;

/**
 * @struct include_dir_t
 * @brief Represents a single include directory entry.
 */
typedef struct {
    char* path;       /* Normalized absolute or TU-rooted path */
    include_type_t type;       /* Include type (user/system) */
    bool           recursive;  /* If true, discover headers recursively */
} include_dir_t;

/**
 * @struct include_dir_vec_t
 * @brief Dynamic array of include directories, preserving insertion order.
 */
typedef struct {
    include_dir_t* v;   /* Pointer to array of include_dir_t */
    size_t         len; /* Number of valid entries */
    size_t         cap; /* Allocated capacity */
} include_dir_vec_t;

/**
 * @struct include_search_path_t
 * @brief Collection of include directories organized by search category.
 *
 * @details Encapsulates all search paths visible to a translation unit, including:
 * - User directories (-I)
 * - System directories (-isystem)
 */
typedef struct {
    //struct DirEntriesCache* dir_cache; /* Optional cache for directory entries */

    include_dir_vec_t user_dirs;       /* -I directories */
    include_dir_vec_t system_dirs;     /* -isystem directories */
    
    char* current_dir;                 /* Current file directory (for quoted includes) */

    bool case_insensitive_fs;          /* True if filesystem is case-insensitive */
} include_search_path_t;

// -----------------------------------------------------------------------------------------------------------------

#endif // _INCLUDE_MAP_H

// -----------------------------------------------------------------------------------------------------------------
