#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include <stdbool.h>

//ENUMS
typedef enum { //for what type of sub-node an ASTNode contains
    NODE_VAR_DECL,
    NODE_VAR_ASSIGN,
    NODE_FUNC_DECL, 
    NODE_WHILE,
    NODE_IF,
    NODE_RETURN
} NodeType;
typedef enum { //for all operators (divide by 4 to get precedence (backward))
    // Operands
    VAL_FUNC = -3,      //function call
    VAL_VAR = -2,       //variable
    VAL_NUM = -1,       //number
    // Special
    NULL_OP = 0,        // error
    // Precedence 1 - parentheses (a separator)
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
typedef enum { //for what type of value a ValueNode contains
    VALUE_OP,           //OperatorType*
    VALUE_NUM,          //long long*
    VALUE_VAR,          //VarEntry*
    VALUE_FUNC_RET,     //FuncCallNode*
    VALUE_MATH_OP,      //MathOpNode*
} ValueType;
typedef enum {
    SCOPE_GLOBAL,       //for global scope
    SCOPE_PARAM_FUNC,   //for functions that have parameters
    SCOPE_OTHER         //for all other scopes
} ScopeInfo;

//STRUCTS
    //HELPER STRUCTS
typedef struct ValueNode { //container for any type of value
    ValueType valueType;
    void* value; //unique pointer to avoid double free from tokens
} ValueNode;
typedef struct queueOrStackNode queueOrStackNode;
typedef struct queueOrStackNode { //queue/stack for converting to and from postfix
    ValueNode* value;
    queueOrStackNode* next;
} queueOrStackNode;
typedef struct FuncCallNode { //container for function calls
    char* funcName;
    int argCount;
    ValueNode** args;
} FuncCallNode;
typedef struct MathOpNode { //node in math expression binary tree
    OperatorType opType;
    ValueNode* left; //nullable for unary operators
    ValueNode* right; 
} MathOpNode;
    //AST STRUCTS
typedef struct ASTNode ASTNode;
typedef struct ASTNode { //linked list of containers for AST nodes (main data structure)
    NodeType nodeType;
    void* subNode;
    ASTNode* next;
    ASTNode* prev;
} ASTNode;
typedef struct VarDeclNode { //sub-node for variable declarations
    VarEntry* varInfo;
} VarDeclNode;
typedef struct VarAssignNode { //sub-node for variable re-assignments
    VarEntry* varInfo;
    ValueNode* newValue;
} VarAssignNode;
typedef struct FuncDeclNode { //sub-node for function declarations
    FuncEntry* funcInfo;
    VarEntry** paramList;
    ASTNode* body;
} FuncDeclNode;
typedef struct WhileNode { //sub-node for while loops
    ValueNode* condition;
    ASTNode* body;
} WhileNode;
typedef struct IfNode { //sub-node for if statements
    ValueNode* condition;
    ASTNode* body;
    ASTNode* elseBody; //nullable for no else
} IfNode;
typedef struct ReturnNode { //sub-node for return statements
    ValueNode* returnValue;
} ReturnNode;

//FUNCTIONS
void freeASTNodes(ASTNode* head);
void printASTs(ASTNode* head, int tabs);
ASTNode* parseTokens(Token* head, ScopeInfo scopeData, VarTable* varTable, FuncTable* funcTable);

#endif