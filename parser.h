#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include <stdbool.h>

typedef enum {
    NODE_VAR_DECL,
    NODE_VAR_ASSIGN,
    NODE_FUNC_DECL, 
    NODE_WHILE,
    NODE_IF,
} NodeType;

typedef enum { //divide by 4 to get precedence level - PRECEDENCE IS BACKWARDS (oopsies)
    I_HAVE_TO_FORCE_THIS_TO_BE_A_SIGNED_TYPE = -1, //this is why -1 >= 0 was true
    NULL_OP = 0,        // error operator
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

typedef enum {
    VALUE_OP,       //OperatorType*
    VALUE_NUM,      //long long*
    VALUE_VAR,      //char*
    VALUE_FUNC_RET, //FuncCallNode*
    VALUE_MATH_OP,  //MathOpNode*
} ValueType;
typedef struct ValueNode {
    ValueType valueType;
    void* value; //should be a unique pointer to avoid double free from tokens
} ValueNode;
typedef struct FuncCallNode {
    char* funcName;
    int argCount;
    ValueNode** args;
} FuncCallNode;
typedef struct MathOpNode {
    OperatorType opType;
    ValueNode* left; //nullable for unary operators
    ValueNode* right; 
} MathOpNode;

typedef struct ASTNode ASTNode;
typedef struct ASTNode {
    NodeType nodeType;
    void* subNode;
    ASTNode* next;
    ASTNode* prev;
} ASTNode;
typedef struct VarDeclNode {
    char* varName;
} VarDeclNode;
typedef struct VarAssignNode {
    char* varName;
    ValueNode* newValue;
} VarAssignNode;
typedef struct FuncDeclNode {
    char* funcName;
    int argCount;
    char** argNames;
    ASTNode* body;
} FuncDeclNode;
typedef struct WhileNode {
    ValueNode* condition;
    ASTNode* body;
} WhileNode;
typedef struct IfNode {
    ValueNode* condition;
    ASTNode* body;
    ASTNode* elseBody; //nullable for no else
} IfNode;

void freeASTNodes(ASTNode* head);
void printASTs(ASTNode* head);
ASTNode* parseTokens(Token* head, VarTable* varTable, FuncTable* funcTable);
MathOpNode* parseMathOp(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable); //will be private later (testing now)

#endif