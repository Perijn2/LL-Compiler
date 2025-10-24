// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef PARSER_CORE_H
#define PARSER_CORE_H

// ------------------------------------------------------------------------------------------------
// Version Information
// ------------------------------------------------------------------------------------------------

#define PARSER_CORE_VERSION_MAJOR 1
#define PARSER_CORE_VERSION_MINOR 0
#define PARSER_CORE_VERSION_PATCH 0

#define PARSER_CORE_VERSION_STRING "1.0.0"

#define PARSER_CORE_VERSION (PARSER_CORE_VERSION_MAJOR * 10000 + PARSER_CORE_VERSION_MINOR * 100 + PARSER_CORE_VERSION_PATCH)

#define PARSER_CORE_STRINGIFY(x) #x
#define PARSER_CORE_TOSTRING(x) PARSER_CORE_STRINGIFY(x)

#define CORE_VERSION_STRING   PARSER_CORE_TOSTRING(CORE_MAJOR) "." \
                              PARSER_CORE_TOSTRING(CORE_MINOR) "." \
                              PARSER_CORE_TOSTRING(CORE_PATCH)
#define PARSER_CORE_FULL_VERSION_STRING \
	"PARSER_CORE Version " CORE_VERSION_STRING

// ----------------------------------------------------------------
// Platform and Calling Conventions
// ----------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#define PARSER_ATTR  
#define PARSER_CALL  __stdcall
#define PARSER_PTR   __stdcall

#elif defined(__GNUC__) || defined(__clang__)

#define PARSER_ATTR  __attribute__((visibility("default")))
#define PARSER_CALL  
#define PARSER_PTR   

#else

#define PARSER_ATTR  
#define PARSER_CALL  
#define PARSER_PTR   

#endif

// Alignment macros
#if defined(__GNUC__) || defined(__clang__)
#define ML_ALIGN(n)     __attribute__((aligned(n)))
#elif defined(_MSC_VER)
#define ML_ALIGN(n)     __declspec(align(n))
#else
#define ML_ALIGN(n)
#endif

// ----------------------------------------------------------------
// Memory Management Hooks
// ----------------------------------------------------------------

#include <stdlib.h>

// Function pointer typedefs for allocation and freeing
typedef void* (PARSER_PTR* PFN_ParserAllocationFunction)(void* pUserData, size_t size, size_t alignment);
typedef void  (PARSER_PTR* PFN_ParserFreeFunction)(void* pUserData, void* pMemory);

// Struct holding user data and function pointers
typedef struct ParserAllocationCallbacks {
    void* pUserData;
    PFN_ParserAllocationFunction pfnAllocation;
    PFN_ParserFreeFunction pfnFree;
} ParserAllocationCallbacks;

// Global pointer to current allocator callbacks (set this at runtime)
extern ParserAllocationCallbacks* g_parser_allocator_callbacks;

#if defined(PARSER_DEBUG_ALLOCATORS)

// Safe macros that check for null pointers before calling
#define PARSER_MALLOC_SAFE(size, alignment) \
        ((g_parser_allocator_callbacks && g_parser_allocator_callbacks->pfnAllocation) ? \
         g_parser_allocator_callbacks->pfnAllocation(g_parser_allocator_callbacks->pUserData, (size), (alignment)) : NULL)

#define PARSER_FREE_SAFE(ptr) \
        do { \
            if (g_parser_allocator_callbacks && g_parser_allocator_callbacks->pfnFree) { \
                g_parser_allocator_callbacks->pfnFree(g_parser_allocator_callbacks->pUserData, (ptr)); \
            } \
        } while(0)

    // Direct macros without safety checks (assumes callbacks valid)
#define PARSER_MALLOC(size, alignment) \
        (g_parser_allocator_callbacks->pfnAllocation(g_parser_allocator_callbacks->pUserData, (size), (alignment)))

#define PARSER_FREE(ptr) \
        (g_parser_allocator_callbacks->pfnFree(g_parser_allocator_callbacks->pUserData, (ptr)))

#else

// Default to standard malloc/free if no custom allocator or debug disabled
#define PARSER_MALLOC_SAFE(size, alignment) malloc(size)
#define PARSER_FREE_SAFE(ptr) free(ptr)
#define PARSER_MALLOC(size, alignment) malloc(size)
#define PARSER_FREE(ptr) free(ptr)

#endif

// ------------------------------------------------------------------------------------------------
// Core Definitions
// ------------------------------------------------------------------------------------------------

#define PARSER_BIT(n) (1 << (n))

#define PARSER_CORE_DEFINE_HANDLE(name) typedef struct name##_T* name;

#include <stdint.h>

typedef uint32_t ParserFlags;
typedef uint32_t ParserResult;
typedef uint64_t ParserSize;
typedef int64_t  ParserSSize;

// ------------------------------------------------------------------------------------------------
#endif // !PARSER_CORE_H
// ------------------------------------------------------------------------------------------------