#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include <stdbool.h>

typedef enum {
    NODE_VAR_DECL,
    NODE_ARRAY_DECL,
    NODE_VAR_ASSIGN,
    NODE_FUNC, 
    NODE_WHILE,
    NODE_IF,
    NODE_MATH_OP
} NodeType;
typedef enum { //divide by 4 to get precedence level
    // Special
    OP_ASSIGN = 0,      // = (only for assignment and declaration)
    // Precedence 1 - parentheses (a separator) -------- FIRST PRECEDENCE
    OP_OPEN_PAREN = 1,  // (
    OP_CLOSE_PAREN = 2, // )
    // Precedence 2 (r->l): 4-7
    OP_BIT_NOT = 4,     // ~
    OP_NOT = 5,         // !
    OP_REF = 6,         // &
    OP_DEREF = 7,       // *
    // Precedence 3 (l->r): 8-11
    OP_MUL = 8,         // *
    OP_DIV = 9,         // /
    OP_MOD = 10,        // %
    // Precedence 4 (l->r): 12-15
    OP_ADD = 12,        // +
    OP_SUB = 13,        // -
    // Precedence 5 (l->r): 16-19
    OP_BIT_L = 16,      // <<
    OP_BIT_R = 17,      // >>
    // Precedence 6 (l->r): 20-23
    OP_LT = 20,         // <
    OP_GT = 21,         // >
    OP_LTE = 22,        // <=
    OP_GTE = 23,        // >=
    // Precedence 7 (l->r): 24-27
    OP_EQU = 24,        // ==
    OP_NEQ = 25,        // !=
    // Precedence 8 (l->r): 28-31
    OP_BIT_AND = 28,    // &
    // Precedence 9 (l->r): 32-35
    OP_BIT_XOR = 32,    // ^
    // Precedence 10 (l->r): 36-39
    OP_BIT_OR = 36,     // |
    // Precedence 11 (l->r): 40-43
    OP_AND = 40,        // &&
    // Precedence 12 (l->r): 44-47
    OP_OR = 44          // ||
} OperatorType;
typedef enum {
    VALUE_NUM,      //64 bit integer
    VALUE_VAR,      //char* varName
    VALUE_FUNC_RET, //char* funcName
    VALUE_MATH_OP   //MathOpNode* mathOp
} ValueType;

typedef struct ASTNode {
    NodeType nodeType;
    void* subNode;
    struct ASTNODE* left;
    struct ASTNODE* right;
} ASTNode;
typedef struct MathOpNode {
    ValueType leftType;
    void* left;
    OperatorType op;
    ValueType rightType;
    void* right;
} MathOpNode;
typedef struct VarDeclNode {
    char* varName;
} VarDeclNode;
typedef struct ArrayDeclNode {
    char* arrayName;
    int arraySize; //number of 64 bit chunks to allocate
} ArrayDeclNode;
typedef struct VarAssignNode {
    char* varName;
    MathOpNode* mathOp;
    void* value;
} VarAssignNode;
typedef struct FuncNode {
    char* funcName;
    int argCount;
    char** argNames;
    ASTNode* body;
} FuncNode;
typedef struct WhileNode {
    MathOpNode* mathOp;
    void* condition;
    ASTNode* body;
} WhileNode;
typedef struct IfNode {
    MathOpNode* mathOp;
    void* condition;
    ASTNode* body;
    ASTNode* elseBody; //NULL if no else
} IfNode;

void freeASTNodes(ASTNode* head);
void printASTs(ASTNode* head);
ASTNode* parseTokens(Token* head);

#endif