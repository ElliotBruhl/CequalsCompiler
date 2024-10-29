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
typedef enum {
    //Special
    OP_NONE,        // Null operator for leaf nodes of math operations
    OP_ASSIGN,      // = (only for assignment and declaration)
    //Precedence 1 - parentheses (a seperator)
    //Precedence 2 (r->l)
    OP_BIT_NOT,     // ~
    OP_NOT,         // !
    OP_REF,         // &
    OP_DEREF,       // *
    //Precedence 3 (l->r)
    OP_MUL,         // *
    OP_DIV,         // /
    OP_MOD,         // %
    //Precedence 4 (l->r)
    OP_ADD,         // +
    OP_SUB,         // -
    //Precedence 5 (l->r)
    OP_BIT_L,       // <<
    OP_BIT_R,       // >>
    //Precedence 6 (l->r)
    OP_LT,          // <
    OP_GT,          // >
    OP_LTE,         // <=
    OP_GTE,         // >=
    //Precedence 7 (l->r)
    OP_EQU,         // ==
    OP_NEQ,         // !=
    //Precedence 8 (l->r)
    OP_BIT_AND,     // &
    //Precedence 9 (l->r)
    OP_BIT_XOR,     // ^
    //Precedence 10 (l->r)
    OP_BIT_OR,      // |
    //Precedence 11 (l->r)
    OP_AND,         // &&
    //Precedence 12 (l->r)
    OP_OR           // ||
} OperatorType;
typedef enum {
    VALUE_NUM,
    VALUE_VAR,
    VALUE_FUNC_RET,
    VALUE_MATH_OP
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