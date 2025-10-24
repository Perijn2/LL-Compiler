// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef PARSER_LANGUAGE_C_H
#define PARSER_LANGUAGE_C_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "parser/ParserCore.h"
#include "parser/Results.h"

#include "parser/Parser.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

extern const ParserLanguageStrategy g_CLanguageStrategy;

// ===== C Binary Operators (for AST_NODE_TYPE_BINARY_EXPR) =====
typedef enum ParserCBinaryOperator {
    C_BINARY_OP_NONE = 0,

    // Arithmetic
    C_BINARY_OP_ADD,              // +
    C_BINARY_OP_SUBTRACT,         // -
    C_BINARY_OP_MULTIPLY,         // *
    C_BINARY_OP_DIVIDE,           // /
    C_BINARY_OP_MODULO,           // %

    // Bitwise
    C_BINARY_OP_BITWISE_AND,      // &
    C_BINARY_OP_BITWISE_OR,       // |
    C_BINARY_OP_BITWISE_XOR,      // ^
    C_BINARY_OP_SHIFT_LEFT,       // <<
    C_BINARY_OP_SHIFT_RIGHT,      // >>

    // Logical
    C_BINARY_OP_LOGICAL_AND,      // &&
    C_BINARY_OP_LOGICAL_OR,       // ||

    // Comparison/Relational
    C_BINARY_OP_EQUAL,            // ==
    C_BINARY_OP_NOT_EQUAL,        // !=
    C_BINARY_OP_LESS,             // <
    C_BINARY_OP_GREATER,          // >
    C_BINARY_OP_LESS_EQUAL,       // <=
    C_BINARY_OP_GREATER_EQUAL,    // >=

    // Assignment
    C_BINARY_OP_ASSIGN,           // =
    C_BINARY_OP_ADD_ASSIGN,       // +=
    C_BINARY_OP_SUB_ASSIGN,       // -=
    C_BINARY_OP_MUL_ASSIGN,       // *=
    C_BINARY_OP_DIV_ASSIGN,       // /=
    C_BINARY_OP_MOD_ASSIGN,       // %=
    C_BINARY_OP_AND_ASSIGN,       // &=
    C_BINARY_OP_OR_ASSIGN,        // |=
    C_BINARY_OP_XOR_ASSIGN,       // ^=
    C_BINARY_OP_SHL_ASSIGN,       // <<=
    C_BINARY_OP_SHR_ASSIGN,       // >>=

    // Comma
    C_BINARY_OP_COMMA,            // ,
} ParserCBinaryOperator;

// ===== C Unary Operators (for AST_NODE_TYPE_UNARY_EXPR) =====
typedef enum ParserCUnaryOperator {
    C_UNARY_OP_NONE = 0,

    C_UNARY_OP_PLUS,              // +x
    C_UNARY_OP_MINUS,             // -x
    C_UNARY_OP_LOGICAL_NOT,       // !x
    C_UNARY_OP_BITWISE_NOT,       // ~x
    C_UNARY_OP_PRE_INCREMENT,     // ++x
    C_UNARY_OP_PRE_DECREMENT,     // --x
    C_UNARY_OP_POST_INCREMENT,    // x++
    C_UNARY_OP_POST_DECREMENT,    // x--
    C_UNARY_OP_ADDRESS_OF,        // &x
    C_UNARY_OP_DEREFERENCE,       // *x
    C_UNARY_OP_SIZEOF,            // sizeof(x)
} ParserCUnaryOperator;

/**
 * @brief C type qualifier flags for variable and pointer declarations
 *
 * @description Type qualifiers modify the properties of objects and pointers in C.
 *              These flags can be combined using bitwise OR operations to specify
 *              multiple qualifiers on a single declaration (e.g., const volatile).
 *              Each qualifier has specific semantic meaning enforced during compilation.
 *
 * @note These are bit flags and can be combined: (C_TYPE_QUAL_CONST | C_TYPE_QUAL_VOLATILE)
 *
 * @see ParserCDeclSpec, ParserCPointerQualifiers
 */
typedef enum ParserCTypeQualifier {
    /**
     * @brief No type qualifiers present
     */
    C_TYPE_QUAL_NONE = PARSER_BIT(0),

    /**
     * @brief Const qualifier - marks object as read-only after initialization
     *
     * Prevents modification of the qualified object. For pointers, can qualify
     * either the pointer itself or the pointed-to data depending on position.
     *
     * @example
     * const int x = 5;        // x cannot be modified
     * int* const ptr = &x;    // ptr cannot be reassigned
     * const int* ptr2 = &x;   // *ptr2 cannot be modified
     */
    C_TYPE_QUAL_CONST = PARSER_BIT(1),

    /**
     * @brief Volatile qualifier - prevents compiler optimizations
     *
     * Indicates that the value may change unexpectedly (e.g., hardware registers,
     * signal handlers, or multi-threaded contexts). Compiler must reload the value
     * on each access rather than caching it in registers.
     *
     * @example
     * volatile int* ioPort = (volatile int*)0x40001000;  // memory-mapped I/O
     */
    C_TYPE_QUAL_VOLATILE = PARSER_BIT(2),

    /**
     * @brief Restrict qualifier - pointer aliasing hint for optimization (C99)
     *
     * Promises that for the lifetime of the pointer, only it or values directly
     * derived from it will be used to access the pointed-to object. Enables
     * aggressive compiler optimizations by eliminating aliasing checks.
     *
     * @note Only valid on pointer types
     * @since C99
     *
     * @example
     * void copy(int* restrict dest, const int* restrict src, size_t n);
     */
    C_TYPE_QUAL_RESTRICT = PARSER_BIT(4),
} ParserCTypeQualifier;

// ===== C Storage Class Specifiers =====
typedef enum ParserCStorageClass {
    C_STORAGE_CLASS_NONE = 0,
    C_STORAGE_CLASS_AUTO,         // auto (rarely used)
    C_STORAGE_CLASS_REGISTER,     // register
    C_STORAGE_CLASS_STATIC,       // static
    C_STORAGE_CLASS_EXTERN,       // extern
    C_STORAGE_CLASS_TYPEDEF,      // typedef (technically a storage class)
} ParserCStorageClass;

// ===== C Function Specifiers =====
typedef enum ParserCFunctionSpecifier {
    C_FUNC_SPEC_NONE = 0x00,
    C_FUNC_SPEC_INLINE = 0x01,    // inline (C99)
} ParserCFunctionSpecifier;

// ===== C Basic Type Specifiers =====
typedef enum ParserCTypeSpecifier {
    C_TYPE_SPEC_NONE = 0,

    // Integer types
    C_TYPE_SPEC_VOID,
    C_TYPE_SPEC_CHAR,
    C_TYPE_SPEC_SHORT,
    C_TYPE_SPEC_INT,
    C_TYPE_SPEC_LONG,
    C_TYPE_SPEC_LONG_LONG,        // long long (C99)

    // Signedness
    C_TYPE_SPEC_SIGNED,
    C_TYPE_SPEC_UNSIGNED,

    // Floating-point
    C_TYPE_SPEC_FLOAT,
    C_TYPE_SPEC_DOUBLE,

    // Boolean (C99)
    C_TYPE_SPEC_BOOL,             // _Bool

    // Complex (C99)
    C_TYPE_SPEC_COMPLEX,          // _Complex

    // User-defined types
    C_TYPE_SPEC_STRUCT,
    C_TYPE_SPEC_UNION,
    C_TYPE_SPEC_ENUM,
    C_TYPE_SPEC_TYPEDEF_NAME,     // user-defined type via typedef
} ParserCTypeSpecifier;

// ===== C Struct/Union Member Access =====
typedef enum ParserCMemberAccessType {
    C_MEMBER_ACCESS_NONE = 0,
    C_MEMBER_ACCESS_DOT,          // struct.member
    C_MEMBER_ACCESS_ARROW,        // ptr->member
} ParserCMemberAccessType;

// ===== C Declaration Specifiers (combined flags) =====
typedef struct ParserCDeclSpec {
    ParserCStorageClass storageClass;
    ParserCTypeQualifier typeQualifiers;  // can be ORed together
    ParserCFunctionSpecifier funcSpecs;   // can be ORed together
    ParserCTypeSpecifier typeSpec;
} ParserCDeclSpec;

// ===== C Linkage Type =====
typedef enum ParserCLinkage {
    C_LINKAGE_NONE = 0,
    C_LINKAGE_INTERNAL,           // static
    C_LINKAGE_EXTERNAL,           // extern or default for functions
    C_LINKAGE_NO_LINKAGE,         // local variables
} ParserCLinkage;

// ===== C Variable/Function Attributes (for extensions) =====
typedef enum ParserCAttribute {
    C_ATTR_NONE = 0x00,
    C_ATTR_PACKED = 0x01,         // __attribute__((packed))
    C_ATTR_ALIGNED = 0x02,        // __attribute__((aligned))
    C_ATTR_NORETURN = 0x04,       // __attribute__((noreturn))
    C_ATTR_DEPRECATED = 0x08,     // __attribute__((deprecated))
    C_ATTR_UNUSED = 0x10,         // __attribute__((unused))
} ParserCAttribute;

// ===== C Pointer Qualifiers =====
typedef struct ParserCPointerQualifiers {
    ParserCTypeQualifier qualifiers;  // const, volatile, restrict on pointer itself
    uint32_t indirectionLevel;        // number of * (e.g., int** has level 2)
} ParserCPointerQualifiers;

// ===== C Array Size Type =====
typedef enum ParserCArraySizeType {
    C_ARRAY_SIZE_FIXED,           // int arr[10]
    C_ARRAY_SIZE_VARIABLE,        // int arr[n] (VLA - C99)
    C_ARRAY_SIZE_UNSPECIFIED,     // int arr[] (declaration or parameter)
    C_ARRAY_SIZE_STATIC,          // int arr[static 10] (parameter)
} ParserCArraySizeType;

// ===== C Operator Precedence Levels =====
typedef enum ParserCPrecedence {
    C_PREC_NONE = 0,
    C_PREC_COMMA,                 // ,
    C_PREC_ASSIGNMENT,            // = += -= *= /= etc.
    C_PREC_CONDITIONAL,           // ?:
    C_PREC_LOGICAL_OR,            // ||
    C_PREC_LOGICAL_AND,           // &&
    C_PREC_BITWISE_OR,            // |
    C_PREC_BITWISE_XOR,           // ^
    C_PREC_BITWISE_AND,           // &
    C_PREC_EQUALITY,              // == !=
    C_PREC_RELATIONAL,            // < > <= >=
    C_PREC_SHIFT,                 // << >>
    C_PREC_ADDITIVE,              // + -
    C_PREC_MULTIPLICATIVE,        // * / %
    C_PREC_UNARY,                 // ! ~ ++ -- + - * & sizeof
    C_PREC_POSTFIX,               // () [] -> . ++ --
    C_PREC_PRIMARY,               // literals, identifiers, ()
} ParserCPrecedence;

typedef ParserResult(PARSER_PTR* PFN_ParsePrimaryExpression)(ASTParser parser, ASTNode node);

PARSER_ATTR ParserResult PARSER_CALL ParserCParsePrimaryExpression(
    ASTParser parser,
    ASTNode node);

// ------------------------------------------------------------------------------------------------
#endif // !PARSER_LANGUAGE_C_H
// ------------------------------------------------------------------------------------------------