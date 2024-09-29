#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>

typedef enum {
    NODE_MATH_OP,
    NODE_VAR_DECL,
    NODE_VAR_ASSIGN,
    NODE_FUNC, 
    NODE_WHILE,
    NODE_IF,
} NodeType;
typedef enum {
    // BINARY OPERATORS
        // Arithmetic
    OP_BINARY_ADD,       // +
    OP_BINARY_SUB,       // -
    OP_BINARY_MUL,       // *
    OP_BINARY_DIV,       // /
    OP_BINARY_MOD,       // %
        // Comparison
    OP_BINARY_LT,        // <
    OP_BINARY_GT,        // >
    OP_BINARY_LTE,       // <=
    OP_BINARY_GTE,       // >=
    OP_BINARY_NEQ,       // !=
    OP_BINARY_EQU,       // ==
        // Bitwise
    OP_BINARY_AND,       // &
    OP_BINARY_OR,        // |
    OP_BINARY_XOR,       // ^
        // Logical
    OP_LOGICAL_AND,      // &&
    OP_LOGICAL_OR,       // ||
    // UNARY OPERATORS
    OP_UNARY_NOT,        // !
    OP_UNARY_BIT_NOT,    // ~
    // ASSIGNMENT OPERATORS
    OP_ASSIGN,           // =
    OP_ASSIGN_ADD,       // +=
    OP_ASSIGN_SUB,       // -=
    OP_ASSIGN_MUL,       // *=
    OP_ASSIGN_DIV,       // /=
    OP_ASSIGN_MOD,       // %=
    OP_ASSIGN_AND,       // &=
    OP_ASSIGN_OR,        // |=
    OP_ASSIGN_XOR,       // ^=
    OP_ASSIGN_BIT_NOT    // ~=
} OperatorType;
typedef struct { //use when operator found
    OperatorType opType; //number each operator to avoid string comparisons

    bool isLeftToken; //is left a nested expression or leaf node token (number or identifier)
    union {
        struct Token *leftToken;
        struct MathOpNode *leftNode; 
    } left;
    bool isRightToken; //is right a nested expression or leaf node token (number or identifier)
    union {
        struct Token *rightToken;
        struct MathOpNode *rightNode;
    } right;
} MathOpNode;
typedef struct { //use when var keyword and '=' found
    unsigned int nameHash;
    bool isToken; //token or mathOp stored in union
    union {
        struct Token *token;
        struct MathOpNode *node;
    } value;
} VarDeclNode;
typedef struct {
    unsigned int nameHash;
    OperatorType opType;
    bool isToken; //token or mathOp stored in union
    union {
        struct Token *token;
        struct MathOpNode *node;
    } value;
} VarAssignNode;
typedef struct FuncNode { //use when func keyword found
    unsigned int nameHash;
    int numArgs;
    int* args;
    //body start/end?
} FuncNode;
typedef struct WhileNode { //use when while keywork found
    bool isExprToken; //is it a nested expression or leaf node token (number or identifier)
    union {
        struct Token *exprToken;
        struct MathOpNode *exprNode; 
    } condition;
    //body start/end?
} WhileNode;
typedef struct IfNode {
    bool isExprToken;
    union {
        struct Token *exprToken;
        struct MathOpNode *exprNode; 
    } condition;
    //body start/end?
    bool hasElse;
    //else start/end?
} IfNode;
typedef struct ASTNode { //stores in linked list structure: which type of node stored, pointer to current, and pointer to next
    NodeType type; //node type
    union {
        MathOpNode* mathOpPtr;
        VarDeclNode* varDeclPtr;
        VarAssignNode* varAssignPtr;
        FuncNode* funcPtr;
        WhileNode* whilePtr;
        IfNode* ifPtr;
    } node;
    struct ASTNode* prev;
    struct ASTNode* next;
} ASTNode;

void freeASTNodes(ASTNode* head);
ASTNode* parseTokens(Token* head);

#endif