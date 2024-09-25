#ifndef PARSER_H
#define PARSER_H

//individal node types pointed to by main linked list
//define each: todo
typedef struct MathOpNode MathOpNode;
typedef struct VarNode VarNode;
typedef struct FuncNode FuncNode;
typedef struct WhileNode WhileNode;
typedef struct IfNode IfNode;
typedef struct ElseNode ElseNode;
typedef struct RetNode RetNode;
typedef struct ContNode ContNode;
typedef struct BreakNode BreakNode;

typedef struct ASTNodeLL { //stores in linked list structure: which type of node stored, pointer to current, and pointer to next
    int type; //node type
    union {
        MathOpNode* mathOpPtr;
        VarNode* varPtr;
        FuncNode* funcPtr;
        WhileNode* whilePtr;
        IfNode* ifPtr;
        ElseNode* elsePtr;
        RetNode* retPtr;
        ContNode* contPtr;
        BreakNode* breakPtr;
    } node;
    struct ASTNodeLL* next;
} ASTNodeLL;

#endif