// -----------------------------------------------------------------------------------------------------------------
//  @file    ast_node.h
//  @author  Perijn Huijser
//  @date    2026-08-30
//  @version 1.0
//
//  @brief This header provides the context and definitions for the AST nodes. 
//  It is used to construct and manipulate ASTs during parsing and semantic analysis.
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
//  - Added definitions and include guard
//  - Created enums and structs for AST nodes
//  - Added comments and documentation
// 
// -----------------------------------------------------------------------------------------------------------------
// Include guard
// -----------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef AST_NODE_H_
#define AST_NODE_H_

// -----------------------------------------------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------------------------------------------

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

/* Standard headers */
#include <stdint.h>

/**
* @enum ast_kind_t
* 
* @brief Enumeration of different kinds of AST nodes.
* 
* @details
* This enum defines various kinds of AST types that can be found in the 
* source code. These enums are the building blocks for the ASM code.
*/
typedef enum {
    AST_INVALID = 0,

    /* declarations / types */
    AST_VAR_DECL,          /* variable declaration */
    AST_PARAM,             /* function parameter */
    AST_TYPE_SPEC,         /* e.g., 'int', 'const T*' */
    AST_PROTO,             /* function prototype */
    AST_FUNC,              /* function definition */

    /* statements */
    AST_BLOCK,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_RETURN,
    AST_BREAK,
    AST_CONTINUE,

    /* expressions */
    AST_IDENT,
    AST_INT_LIT,
    AST_FLOAT_LIT,
    AST_STRING_LIT,
    AST_UNARY,
    AST_BINARY,
    AST_ASSIGN,
    AST_CALL,
    AST_INDEX,
    AST_CAST,

    AST_SCOPE,             /* optional: lexical scope wrapper */
    AST_COUNT
} ast_kind_t;

/**
 * @enum unary_op_t
 * @brief Enumeration of unary operators.
 *
 * This enumeration defines the set of unary operators that may appear in
 * expressions.
 */
typedef enum {
    /**
     * @brief Invalid or uninitialized operator.
     */
    UOP_INVALID = 0,

    /* --------------------------------------------------------------------- */
    /** @name Sign operators */
    /* --------------------------------------------------------------------- */
    UOP_PLUS,     /** Unary plus `+x` */
    UOP_MINUS,    /** Unary minus `-x` */
    
    /* --------------------------------------------------------------------- */
    /** @name Bitwise / logical operators */
    /* --------------------------------------------------------------------- */
    UOP_BIT_NOT,  /** Bitwise NOT `~x` */
    UOP_LOG_NOT,  /** Logical NOT `!x` */


    /* --------------------------------------------------------------------- */
    /** @name Increment / decrement operators */
    /* --------------------------------------------------------------------- */
    UOP_PRE_INC,  /** Pre-increment `++x` */
    UOP_PRE_DEC,  /** Pre-decrement `--x` */
    UOP_POST_INC, /** Post-increment `x++` */
    UOP_POST_DEC, /** Post-decrement `x--` */

    /* --------------------------------------------------------------------- */
    /** @name Addressing / dereferencing operators */
    /* --------------------------------------------------------------------- */
    UOP_ADDR_OF,  /** Address-of `&x` */
    UOP_DEREF,    /** Dereference `*x` */

    /* --------------------------------------------------------------------- */
    /** @name Type trait operators */
    /* --------------------------------------------------------------------- */
    UOP_SIZEOF,   /** Sizeof operator `sizeof x` */
    UOP_ALIGNOF   /** Alignof operator `_Alignof x` */

} unary_op_t;

typedef enum {
    BOP_INVALID = 0,
    BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_MOD,
    BOP_SHL, BOP_SHR,
    BOP_BIT_AND, BOP_BIT_OR, BOP_BIT_XOR,
    BOP_LOG_AND, BOP_LOG_OR,
    BOP_EQ, BOP_NE, BOP_LT, BOP_LE, BOP_GT, BOP_GE,
    BOP_ASSIGN,
    BOP_ADD_ASSIGN, BOP_SUB_ASSIGN, BOP_MUL_ASSIGN, BOP_DIV_ASSIGN, BOP_MOD_ASSIGN,
    BOP_SHL_ASSIGN, BOP_SHR_ASSIGN,
    BOP_AND_ASSIGN, BOP_OR_ASSIGN, BOP_XOR_ASSIGN,
    BOP_MEMBER_DOT, BOP_MEMBER_ARROW
} binary_op_t;

/* Base types (qualifiers live in flags below) */
typedef enum {
    BASE_TYPE_VOID = 0, BASE_TYPE_BOOL,
    BASE_TYPE_CHAR, BASE_TYPE_SHORT, BASE_TYPE_INT, BASE_TYPE_LONG, BASE_TYPE_LONG_LONG,
    BASE_TYPE_FLOAT, BASE_TYPE_DOUBLE, BASE_TYPE_LONG_DOUBLE
} base_type_t;

typedef enum {
    MOD_NONE = 0,
    MOD_STATIC = BIT(0),
    MOD_INLINE = BIT(1),
    MOD_CONST = BIT(2),
    MOD_VOLATILE = BIT(3),
    MOD_RESTRICT = BIT(4),
    MOD_SIGNED = BIT(5),
    MOD_UNSIGNED = BIT(6)
} type_flags_t;


//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

typedef struct ast_node ast_node_t;

/* Type info (minimal for now) */
typedef struct type_info {
    const char* name;      /* typedef or tag, optional */
    base_type_t  base;
    uint16_t     flags;     /* type_flags_t */
    uint32_t     ptr_level; /* number of '*' */
    size_t       array_len; /* 0 = not array; TODO: vector for multi-dim */
} type_info_t;

/* Symbols */
typedef struct symbol {
    const char* name;
    type_info_t* type;
    ast_node_t* value;     /* initializer or const expr, optional */
} symbol_t;

/* Statement/expr payloads */
typedef struct { ast_node_t* stmts; } ast_block_t;

typedef struct {
    const char* name;
    type_info_t* ret_type;
    ast_node_t* params;    /* list of PARAM nodes via next */
} ast_proto_t;

typedef struct { ast_node_t* proto; ast_node_t* body; } ast_func_t;

typedef struct { ast_node_t* cond; ast_node_t* then_b; ast_node_t* else_b; } ast_if_t;
typedef struct { ast_node_t* cond; ast_node_t* body; } ast_while_t;
typedef struct { ast_node_t* init; ast_node_t* cond; ast_node_t* post; ast_node_t* body; } ast_for_t;
typedef struct { ast_node_t* expr; } ast_return_t;

/* Call / index / assignment / ops */
typedef struct { ast_node_t* callee; ast_node_t* args; } ast_call_t;   /* callee now expression */
typedef struct { ast_node_t* base; ast_node_t* index; } ast_index_t;
typedef struct { ast_node_t* lhs; ast_node_t* rhs; } ast_assign_t;
typedef struct { unary_op_t  op; ast_node_t* expr; } ast_unary_t;
typedef struct { binary_op_t op; ast_node_t* lhs; ast_node_t* rhs; } ast_binary_t;

/* Literals / identifiers */
typedef struct {
    type_info_t* type;      /* static type if known */
    union {
        double     f64;
        uint64_t   u64;
        int64_t    i64;
        struct { const char* ptr; size_t len; } str;   /* string literal */
        const char* ident;   /* identifier spelling (interned recommended) */
    } v;
} ast_literal_t;

/* Concrete node */
struct ast_node {
    ast_kind_t kind;
    ast_node_t* next;    /* for lists: params, args, stmts */

    union {
        symbol_t      sym;        /* VAR_DECL */
        type_info_t   type_spec;  /* TYPE_SPEC */
        ast_block_t   block;
        ast_proto_t   proto;
        ast_func_t    func;
        ast_if_t      ifs;
        ast_while_t   whiles;
        ast_for_t     fors;
        ast_return_t  ret;
        ast_call_t    call;
        ast_index_t   index;
        ast_assign_t  assign;
        ast_unary_t   unary;
        ast_binary_t  binary;
        ast_literal_t lit;
    };
};

// -----------------------------------------------------------------------------------------------------------------
#endif // !AST_NODE_H_
// -----------------------------------------------------------------------------------------------------------------