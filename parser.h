#include <stdbool.h>
#ifndef PARSER_H
#define PARSER_H

typedef enum {
    NODE_MATH_OP,
    NODE_VAR,
    NODE_FUNC, 
    NODE_WHILE,
    NODE_IF,   
    NODE_ELSE, 
    NODE_RET,
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

} OperatorType;


typedef struct {
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
typedef struct {
    char* varName;
    bool isToken; //token or mathOp stored in union
    union {
        struct Token *token;
        struct MathOpNode *node;
    } value;
} VarNode;
typedef struct FuncNode FuncNode;
typedef struct WhileNode WhileNode;
typedef struct IfNode IfNode;
typedef struct ElseNode ElseNode;
typedef struct RetNode RetNode;

typedef struct ASTNode { //stores in linked list structure: which type of node stored, pointer to current, and pointer to next
    NodeType type; //node type
    union {
        MathOpNode* mathOpPtr;
        VarNode* varPtr;
        FuncNode* funcPtr;
        WhileNode* whilePtr;
        IfNode* ifPtr;
        ElseNode* elsePtr;
        RetNode* retPtr;
    } node;
    struct ASTNode* next;
} ASTNode;

#endif