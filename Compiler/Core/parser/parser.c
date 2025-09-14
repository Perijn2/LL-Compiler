//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: Parser.h
* Modified: 19/04/2025
*/
//------------------------------------------------------------------------------

#include "Parser.h"

#include <stdlib.h>

//------------------------------------------------------------------------------

static ast_status_t create_parser(compile_unit_t* unit, Arena* arena, ast_parser_t* parser)
{
    //Arena* arena = arena_create(4096);  // 4KB initial block

    Parser* parser = (Parser*) arenaAlloc(arena, sizeof(Parser));
    parser->tokens = tokens;
    parser->size = tokenCount;
    parser->pos = 0;
    parser->current = tokens[0];

    parser->arena = arena;

    parser->global_scope = (ASTScope*)arenaAlloc(parser->arena, sizeof(ASTScope));
    parser->global_scope->symTable = kh_init(symbols);
    parser->global_scope->varCount = 0;
    parser->global_scope->parent = nullptr;

    parser->current_scope = parser->global_scope;

    return parser;
}

// Create a new scope and push onto the stack
static ast_status_t enter_scope(ast_parser_t* parser) {
    ASTScope* new_scope = (ASTScope*) arenaAlloc(parser->arena, sizeof(ASTScope));
    new_scope->symTable = kh_init(symbols);
    new_scope->varCount = 0;

    // Link to parent scope
    new_scope->parent = parser->current_scope;
    parser->current_scope = new_scope;
}

// Pop the current scope and return to parent
void exitScope(Parser* parser) {
    if (parser->current_scope && parser->current_scope->parent) {
        parser->current_scope = parser->current_scope->parent;
    }
}

// Add symbol to current scope
bool addSymbol(Parser* parser, const char* name, ASTNode* node) {
    khash_t(symbols)* tbl = parser->current_scope->symTable;
    int32_t ret;
    khiter_t k = kh_put(symbols, tbl, name, &ret);

    if (ret == -1) return false; // Hash table full
    if (ret == 0) { // Already exists
        ASTNode* existing = kh_value(tbl, k);
        /*printf("Error: '%s' already defined at line %d\n",
            name, existing->location.line);*/
        return false;
    }

    kh_value(tbl, k) = node;
    parser->current_scope->varCount++;
    return true;
}

// Lookup symbol in current and parent scopes
ASTNode* lookupSymbol(Parser* parser, const char* name) {
    ASTScope* scope = parser->current_scope;

    while (scope) {
        khash_t(symbols)* tbl = scope->symTable;
        khiter_t k = kh_get(symbols, tbl, name);
        if (k != kh_end(tbl)) {
            return kh_value(tbl, k);
        }
        scope = scope->parent;
    }
    return NULL; // Not found
}

ASTNode* lookupFunction(Parser* parser, const char* name)
{
    khash_t(symbols)* tbl = parser->funcTable;
    khiter_t k = kh_get(symbols, tbl, name);
    if (k != kh_end(tbl)) {
        return kh_value(tbl, k);
    }
    return NULL;
}

Token* advanceTokenBy(Parser* parser, uint8_t offset = 1)
{
    if (offset == 0) return parser->current;  // No-op

    size_t newPos = parser->pos + offset;
    if (newPos >= parser->size) return nullptr;

    parser->pos = newPos;
    parser->current = parser->tokens[parser->pos];
    return parser->current;
}

Token* expectToken(Parser* parser, TokenType type, const char* errorMessage)
{
    Token* tk = peekToken(parser, 0);
    advanceTokenBy(parser, 1);

    if (tk->type == type) return tk;

    printf(errorMessage);
    return nullptr;
}

Token* peekToken(Parser* parser, uint8_t offset)
{
    if (parser->pos + offset >= parser->size) return nullptr;

    return parser->tokens[parser->pos + offset];
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void pushModifier(Parser* parser, ModifierFlags flag) {
    if (parser->modifiers & flag) {
        //error("Duplicate modifier");
        return;
    }

    parser->modifiers = (ModifierFlags)(parser->modifiers | flag);
}

void applyModifiers(Parser* parser) {
    parser->modifiers = MOD_NONE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ASTNode* parseFunctionCall(Parser* parser)
{
    Token* name = expectToken(parser, TOK_IDENTIFIER, "Expected an identifier");
    if (!name) return NULL;

    ASTNode* func = lookupFunction(parser, name->text);
    if (!func) {
        //reportError(parser, name->location, "Undefined function '%s'", name->text);
        return NULL;
    }

    ASTNode* proto = func->func.prototype;
    if (!proto || proto->type != NODE_PROTO) return NULL;

    if (!expectToken(parser, TOK_LPAREN, "Expected '('")) return NULL;

    ASTNode* args = parseArguments(parser);
    if (!args) return NULL;

    if (!expectToken(parser, TOK_RPAREN, "Expected ')'")) return NULL;

    ASTNode* node = newNode(parser, NODE_CALL);
    if (!node) return NULL;

    node->callExpr.name = name->text;
    node->callExpr.args = args;
    node->callExpr.ret = proto->proto.retType;

    return node;
}



//ASTNode* parseParam(Parser* parser)
//{
//    TypeInfo* dataType = parseDatatype(parser);
//    if (!dataType) return nullptr;
//
//    Token* token = peekToken(parser, 0);
//    if (!token) return nullptr;
//
//    advanceTokenBy(parser);
//
//    ASTNode* node = newNode(parser, NODE_PARAM);  
//
//    if (token->type == TOK_IDENTIFIER)
//    {
//        node->sym.name = token->text;
//        node->sym.type = dataType;
//
//        expectToken(parser, TOK_COMMA, "Exptected a ','");
//
//        return node;
//    }
//    else if (token->type == TOK_COMMA)
//    {
//        node->sym.type = dataType;
//
//        return node;
//    }
//
//    // TODO: error report
//
//    return nullptr;
//}

ASTNode* parseIdentifier(Parser* parser);


/*
    Helper function for parsing the declerations

    Example: <TypeInfo> <DeclName> <Symbol>
    Where:
        TypeInfo is a standard or custom type like: int, double, uint8_t, int16_t, etc.
        DeclName is the name of the decleration
        Symbol this defines what the decleration will be like a function, variable, etc.
*/
ASTNode* parseDecleration(Parser* parser)
{
    if (!parser) return NULL;

    // Looking for a data type
    TypeInfo* dataType = parseDatatype(parser);
    if (!dataType) return NULL;

    // Looking for the declaration name
    Token* name = expectToken(parser, TOK_IDENTIFIER, "Expected an identifier");
    if (!name) return NULL;

    // Peeking the next token
    Token* next = peekToken(parser, 0);
    if (!next) {
        //reportError(parser, "Unexpected end of input after identifier");
        return NULL;
    }

    // Parsing variable decleration
    if (next->type == TOK_OPERATOR && next->subtype == OP_ASSIGN) {
        // Consume the operator token
        advanceTokenBy(parser, 1); // consume '='

        // Parsing the expression of the variable
        ASTNode* value = parseExpression(parser, NULL);
        if (!value) return NULL;

        expectToken(parser, TOK_SEMICOLON, "Expected an ;");

        ASTNode* node = newNode(parser, NODE_VAR_DECL);
        if (!node) return NULL;

        node->sym.name = name->text;
        node->sym.type = dataType;
        node->sym.value = value;

        if (!addSymbol(parser, name->text, node)) {
            //reportError(parser, "Variable '%s' already declared", name->text);
            return NULL;
        }

        return node;
    }
    else if (next->type == TOK_LPAREN) { // Parsing function decleration
        ASTNode* funProto = newNode(parser, NODE_PROTO);
        if (!funProto) return NULL;

        funProto->proto.name = name->text;
        funProto->proto.retType = dataType;

        ASTNode* func = parseFunction(parser, funProto);
        if (!func) return NULL;

        //if (!addFunctionDecleration(parser, name->text, func)) {
        //    //reportError(parser, "Function '%s' already declared", name->text);
        //    return NULL;
        //}

        return func;
    }
    else if (next->type == TOK_SEMICOLON) {
        ASTNode* node = newNode(parser, NODE_VAR_DECL);
        if (!node) return NULL;

        node->sym.name = name->text;
        node->sym.type = dataType;
        node->sym.value = NULL;

        if (!addSymbol(parser, name->text, node)) {
            //reportError(parser, "Variable '%s' already declared", name->text);
            return NULL;
        }

        return node;
    }
    else {
        //reportError(parser, "Unexpected token after identifier '%s'", name->text);
        return NULL;
    }
}

ASTNode* parseFunction(Parser* parser, ASTNode* prototype)
{
    // Consume the operator token
    expectToken(parser, TOK_LPAREN, "Expected an '('");

    ASTNode* params = parseParams(parser);
    prototype->proto.params = params;

    ASTNode* funNode = newNode(parser, NODE_FUNC);
    funNode->func.prototype = prototype;

    Token* nextToken = peekToken(parser, 0);

    if (nextToken->type == TOK_LBRACE)
    {
        ASTNode* body = parseBody(parser);
        if (!body) return NULL;

        funNode->func.body = body;
    }
    else if (nextToken->type == TOK_SEMICOLON)
    {
        funNode->func.body = NULL;
    }

    //addFunctionDecleration(parser, prototype->proto.name, funNode);
    //ASTNode* body = parseBlock(parser);

    return funNode;
}

//ASTNode* parseVariableDeclaration(Parser* parser);

ASTNode* parseIfStatement(Parser* parser)
{
    Token* ifKey = peekToken(parser, 0);
    if (ifKey->type != TOK_KEYWORD || ifKey->subtype != KEYWORD_IF) return NULL;
    advanceTokenBy(parser, 1);

    expectToken(parser, TOK_LPAREN, "Expected '(' after 'if'");

    ASTNode* cond = parseExpression(parser, 0);
    expectToken(parser, TOK_RPAREN, "Expected ')' after if condition");

    ASTNode* thenStmt = parseStatement(parser);
    if (!thenStmt) return NULL;

    ASTNode* ifNode = newNode(parser, NODE_IF_STMT);
    ifNode->ifStmt.cond = cond;
    ifNode->ifStmt.body = thenStmt;
    ifNode->ifStmt.alt = NULL;

    // Handle else
    Token* next = peekToken(parser, 0);
    if (next && next->type == TOK_KEYWORD && next->subtype == KEYWORD_ELSE) {
        advanceTokenBy(parser, 1); // Consume 'else'
        // If 'else if', recurse; else, just parse the next statement/block
        Token* lookahead = peekToken(parser, 0);
        if (lookahead && lookahead->type == TOK_KEYWORD && lookahead->subtype == KEYWORD_IF) {
            ifNode->ifStmt.alt = parseIfStatement(parser);
        }
        else {
            ifNode->ifStmt.alt = parseStatement(parser);
        }
    }
    return ifNode;
}

ASTNode* parseUnary(Parser* parser) {
    Token* token = peekToken(parser, 0);
    if (!token) return NULL;

    // Handle prefix unary operators
    if (token->type == TOK_OPERATOR && isUnary((Operators)token->subtype)) {
        advanceTokenBy(parser, 1);  // consume operator
        ASTNode* operand = parseUnary(parser);  // recursive to support chaining like --*x

        if (!operand) return NULL;

        ASTNode* node = newNode(parser, NODE_UNARY_OP);
        switch (token->subtype) {
        case OP_MIN: node->unary.op_type = UNOP_NEGATE; break;
        case OP_DEC: node->unary.op_type = UNOP_PRE_DEC; break;
        case OP_INC: node->unary.op_type = UNOP_PRE_INC; break;
        default: return NULL;  // Unknown prefix
        }

        node->unary.operand = operand;
        return node;
    }

    // Otherwise parse a primary expression
    ASTNode* node = parsePrimary(parser);
    if (!node) return NULL;

    // Handle postfix operators
    while (true) {
        Token* next = peekToken(parser, 0);
        if (!next || next->type != TOK_OPERATOR) break;

        if (next->subtype == OP_INC || next->subtype == OP_DEC) {
            advanceTokenBy(parser, 1);

            ASTNode* post = newNode(parser, NODE_UNARY_OP);
            post->unary.operand = node;

            if (next->subtype == OP_INC) {
                post->unary.op_type = UNOP_POST_INC;
            }
            else {
                post->unary.op_type = UNOP_POST_DEC;
            }

            node = post;  // chain
        }
        else {
            break;
        }
    }

    return node;
}


ASTNode* parseExpression(Parser* parser, int minPrecedence) {
    ASTNode* left = parsePrimary(parser);
    if (!left) return NULL;

    Token* token = peekToken(parser, 0);

    while (token && token->type == TOK_OPERATOR &&
        getPrecedence((Operators)token->subtype) >= minPrecedence) {

        Operators op = (Operators)token->subtype;
        int opPrecedence = getPrecedence(op);

        advanceTokenBy(parser, 1);  // Consume operator

        // For right-associative operators (like assignment), 
        // you'd pass opPrecedence instead of opPrecedence + 1 here.
        ASTNode* right = parseExpression(parser, opPrecedence + 1);
        if (!right) return NULL;

        // Build binary operation AST node
        ASTNode* binOpNode = newNode(parser, NODE_BINARY_OP);
        binOpNode->binary.left = left;
        binOpNode->binary.right = right;
        binOpNode->binary.op = op;

        left = binOpNode;

        token = peekToken(parser, 0);
    }

    return left;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ASTNode* parseStatement(Parser* parser) {
    Token* token = peekToken(parser, 0);
    if (!token) return nullptr;

    switch (token->type) {
    case TOK_KEYWORD:
        switch ((KeywordType)token->subtype) {
        case KEYWORD_IF:
            return parseIfStatement(parser);
        case KEYWORD_WHILE:
            return nullptr; //parseWhileStatement(parser);
        case KEYWORD_FOR:
            return nullptr;// parseForStatement(parser);
        case KEYWORD_RETURN:
            return nullptr;// parseReturnStatement(parser);
            // Add more as needed
        default:
            // Maybe it's a type keyword (int, float, etc.)
            if (isDatatype((KeywordType)token->subtype)) {
                return parseDecleration(parser);// parseVariableDeclaration(parser);
            }
            break;
        }
        break;

    case TOK_LBRACE:
        return parseBody(parser);

    case TOK_IDENTIFIER:
    case TOK_INTEGER:
    case TOK_FLOAT:
    case TOK_LPAREN:
        // Expression statement (could be assignment, function call, etc.)
    {
        ASTNode* expr = parseExpression(parser, 0);
        expectToken(parser, TOK_SEMICOLON, "Expected ';' after expression");
        return expr;
    }
    // Handle other cases if needed (labels, etc.)
    }

    // If nothing matches, report an error
    // error("Unknown statement");
    return nullptr;
}

TypeInfo* parseDatatype(Parser* parser)
{
    Token* token = peekToken(parser, 0);

    if (token->type == TOK_KEYWORD && isDatatype((KeywordType)token->subtype))
    {
        TypeInfo* type = (TypeInfo*)arenaAlloc(parser->arena, sizeof(TypeInfo));
        if (!type) return nullptr;

        type->baseType = (KeywordType)token->subtype;
        type->name = getKeywordName(type->baseType);
        type->flags = MOD_NONE;

        advanceTokenBy(parser, 1);

        return type;
    }
    else if (token->type == TOK_IDENTIFIER)
    {

    }
}

ASTNode* parsePrimary(Parser* parser) {
    Token* token = peekToken(parser, 0);
    if (!token) return NULL;

    if (token->type == TOK_INTEGER) {
        advanceTokenBy(parser, 1);
        ASTNode* node = newNode(parser, NODE_INT_LIT);
        node->ref.val.uintLit = token->intValue;
        return node;
    }
    else if (token->type == TOK_FLOAT) {
        advanceTokenBy(parser, 1);
        ASTNode* node = newNode(parser, NODE_FLOAT_LIT);
        node->ref.val.floatLit = token->floatValue;
        return node;
    }
    else if (token->type == TOK_IDENTIFIER) {
        // Peek ahead to see if it's a function call
        Token* nextToken = peekToken(parser, 1);
        if (nextToken && nextToken->type == TOK_LPAREN) {
            return parseFunctionCall(parser);
        }
        else {
            // Simple identifier
            advanceTokenBy(parser, 1);
            ASTNode* node = newNode(parser, NODE_IDENTIFIER);
            node->ref.val.ref = token->text;
            return node;
        }
    }
    else if (token->type == TOK_LPAREN) {
        advanceTokenBy(parser, 1); // Consume '('
        ASTNode* expr = parseExpression(parser, 0);
        expectToken(parser, TOK_RPAREN, "Expected ')'");
        return expr;
    }
    else if (token->type == TOK_OPERATOR && isUnary((Operators)token->subtype)) {
        return parseUnary(parser);
    }

    // TODO: Add proper error handling here.
    return NULL;
}

ASTNode* parseBody(Parser* parser)
{
    if (!parser) return NULL;

    expectToken(parser, TOK_LBRACE, "Expected '{'");
    enterScope(parser);

    ASTScope* thisScope = parser->current_scope;  // Save pointer to this scope

    ASTNode* blockNode = newNode(parser, NODE_BLOCK);
    blockNode->block.statements = NULL;
    blockNode->block.scope = parser->current_scope;

    ASTNode** lastPtr = &blockNode->block.statements;

    while (true) {
        Token* tk = peekToken(parser, 0);
        if (!tk || tk->type == TOK_RBRACE) break;

        ASTNode* stmt = parseStatement(parser);
        if (!stmt) break;
        *lastPtr = stmt;
        lastPtr = &stmt->next;
    }

    expectToken(parser, TOK_RBRACE, "Expected '}'");
    exitScope(parser);

    return blockNode;
}

ASTNode* parseParams(Parser* parser)
{
    if (!parser) return nullptr;
    //expectToken(parser, TOK_LPAREN, "Expected '('");

    ASTNode* firstParam = NULL;
    ASTNode* lastParam = NULL;

    while (true)
    {
        // Check for closing parenthesis (end of parameter list)
        Token* tok = peekToken(parser, 0);
        if (tok->type == TOK_RPAREN) {
            advanceTokenBy(parser, 1);
            break;
        }

        ASTNode* paramNode = parsePrimary(parser); //newNode(parser, NODE_PARAM);
        if (!paramNode) return NULL;
        
        if (!firstParam) {
            firstParam = lastParam = paramNode;
        }
        else {
            lastParam->next = paramNode;
            lastParam = paramNode;
        }

        // Check for comma or closing paren
        Token* nextTok = peekToken(parser, 0);
        if (nextTok->type == TOK_COMMA) {
            advanceTokenBy(parser, 1); // Consume comma and continue loop
        }
        else if (nextTok->type == TOK_RPAREN) {
            advanceTokenBy(parser, 1); // Consume closing parenthesis
            break;
        }
        else {
            //reportError(parser, "Expected ',' or ')'");
            return NULL;
        }
    }

    expectToken(parser, TOK_RPAREN, "Expected ')'");

    return firstParam;
}

/* 
    Helper function for parsing the arguments of a method call

    Example: MyMethod(arg1, arg2, (int) arg3, etc)
    This will be parsed as: arg1
                            arg2
                            (int) arg3
                            etc.
*/ 
ASTNode* parseArguments(Parser* parser)
{
    if (!parser) return nullptr;
    if (!expectToken(parser, TOK_LPAREN, "Expected '('")) return NULL;

    ASTNode* firstArgument = NULL;
    ASTNode* lastArgument = NULL;

    while (true)
    {
        // Check for closing parenthesis (end of argument list)
        Token* tok = peekToken(parser, 0);
        if (tok->type == TOK_RPAREN) {
            advanceTokenBy(parser, 1);
            break;
        }

        // Expect an identifier for the argument name
        /*Token* idenTok = expectToken(parser, TOK_IDENTIFIER, "Expected Argument name");
        if (!idenTok) {
            return NULL;
        }*/

        // Create argument node
        //ASTNode* argumentNode = createArgumentNode(typeNode, identTok->text);
        ASTNode* argumentNode = parsePrimary(parser); //newNode(parser, NODE_ARGUMENT);
        if (!argumentNode) return NULL;
        //argumentNode->ref.refType = nullptr;
        // TODO: resolve the base type of the reference
        //argumentNode->ref.litValue.ref = idenTok->text;

        // Link into list
        if (!firstArgument) {
            firstArgument = lastArgument = argumentNode;
        }
        else {
            lastArgument->next = argumentNode;
            lastArgument = argumentNode;
        }

        // Check for comma or closing paren
        Token* nextTok = peekToken(parser, 0);
        if (nextTok->type == TOK_COMMA) {
            advanceTokenBy(parser, 1); // Consume comma and continue loop
        }
        else {
            //reportError(parser, "Expected ',' or ')'");
            return NULL;
        }
    }

    return firstArgument;
}

uint8_t getPrecedence(Operators op) {
    switch (op) {
    case OP_ASSIGN:      return 1;
    case OP_EQ:
    case OP_NEQ:   return 2;
    case OP_LESS:
    case OP_GREATER:
    case OP_LESS_EQUAL:
    case OP_GREATER_EQUAL: return 3;
    case OP_ADD:
    case OP_MIN:         return 4;
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:      return 5;
    default:             return 0;  // unknown operators
    }
}

uint8_t isDatatype(KeywordType type)
{
    switch (type)
    {
    case KEYWORD_CHAR:
    case KEYWORD_SHORT:
    case KEYWORD_INT:
    case KEYWORD_LONG:
    {
        return 1;
    }
    default:
        return 0;
    }
}

uint8_t isLiteral(TokenType type)
{
    switch (type)
    {
    case TOK_FLOAT:
    case TOK_INTEGER:
    case TOK_IDENTIFIER:
        return 1;
    default:
        return 0;
    }
}

uint8_t isUnary(Operators type)
{
    switch (type)
    {
    case OP_DEC:
    case OP_INC:
    case OP_MIN:
        return 1;
    default:
        return 0;
    }
}

ASTNode* newNode(Parser* parser, ASTType type)
{
    ASTNode* newNode = (ASTNode*)arenaAlloc(parser->arena, sizeof(ASTNode));
    memset(newNode, 0, sizeof(ASTNode));
    newNode->type = type;

    parser->current_node = newNode;

    return newNode;
}


//------------------------------------------------------------------------------