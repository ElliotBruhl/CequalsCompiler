#include "parser.h"
#include "identifierHashTable.h"
#include <stdlib.h>

void printASTs(ASTNode* head) {
    //make a good print func
}

void freeMathOpNode(MathOpNode* node) {
    if (node == NULL) {
        return;
    }
    if (!node->isLeftToken) {
        freeMathOpNode(node->left.leftNode);
    }
    if (!node->isRightToken) {
        freeMathOpNode(node->right.rightNode);
    }
    free(node);
}
void freeASTNodes(ASTNode* head) {
    ASTNode* current = head;

    while (current != NULL) {
        ASTNode* next = current->next;
        switch (current->type) {
            case NODE_MATH_OP:
                freeMathOpNode(current->node.mathOpPtr);
                break;
            case NODE_VAR_DECL:
                if (!current->node.varDeclPtr->isToken)
                    freeMathOpNode(current->node.varDeclPtr->value.node);
                free(current->node.varDeclPtr);
                break;
            case NODE_VAR_ASSIGN:
                if (!current->node.varAssignPtr->isToken)
                    freeMathOpNode(current->node.varAssignPtr->value.node);
                free(current->node.varAssignPtr);
                break;
            case NODE_FUNC:
                free(current->node.funcPtr);
                break;
            case NODE_WHILE:
                if (!current->node.whilePtr->isExprToken)
                    freeMathOpNode(current->node.whilePtr->condition.exprNode);
                free(current->node.whilePtr);
                break;
            case NODE_IF:
                if (!current->node.ifPtr->isExprToken)
                    freeMathOpNode(current->node.ifPtr->condition.exprNode);
                free(current->node.ifPtr);
                break;
        }
        free(current);
        current = next;
    }
}
Token* jumpToNext(Token* current) {
    while (current->value[0] != ';' && current->nextToken != NULL)
        current = current->nextToken;
    return current;
}

ASTNode* createNode(NodeType nodeType, void* subNode, ASTNode* prevNode) {
    ASTNode* newNode = (ASTNode*)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        printf("Malloc failed in createNode");
        return NULL;
    }

    newNode->type = nodeType;
    newNode->node.mathOpPtr = subNode; //I hope this doesn't cause problems in the future...
    newNode->prev = prevNode;
    newNode->next = NULL;
    if (prevNode != NULL) {
        prevNode->next = newNode;
    }

    return newNode;
}

VarDeclNode* createVarDecl(Token* tokens) {
    VarDeclNode* node = NULL;
    Token* curTok = tokens->nextToken;

    //check for valid declaration
    if (curTok == NULL || curTok->tokenType != TOKEN_IDENTIFIER) {//next token is identifier
        printf("line %d: No var name found for var declaration.", tokens->lineNum);
        return NULL;
    }
    curTok = curTok->nextToken;
    if (curTok == NULL || curTok->tokenType != TOKEN_OPERATOR || curTok->value[0] != '=') { //next after that is '='
        printf("line %d: No '=' found for var declaration.", tokens->lineNum);
        return NULL;
    }
    curTok = curTok->nextToken;
    if (curTok == NULL) { //next after that is mathOp, number, or identifier
        printf("line %d: No var value found for var declaration.", tokens->lineNum);
        return NULL;
    }
    if ((curTok->tokenType == TOKEN_IDENTIFIER || curTok->tokenType == TOKEN_NUMBER) && curTok->nextToken != NULL && curTok->nextToken->value[0] == ';') { //number or identifier followed by ';'
        node = (VarDeclNode*)malloc(sizeof(VarDeclNode));
        if (node == NULL) {
            printf("malloc failed in creatVarDecl");
            return NULL;
        }
        node->isToken = true;
        node->value.token = curTok;
        node->nameHash = hash(tokens->nextToken->value);
        return node;
    }
    else {
        MathOpNode* opAssigned/* = createMathOpNode()*/;
        if (opAssigned == NULL) {
            printf("line %d: var declaration value cannot be determined.", tokens->lineNum);
            return NULL;
        }
        //create the node
        return node;
    }
}

ASTNode* parseTokens(Token* headToken) {
    NodeType nodeType;
    Token* curToken = headToken;
    ASTNode* headNode = NULL;
    ASTNode* prevNode = NULL;
    void* subNode = NULL;

    while (curToken != NULL) {
        switch (curToken->tokenType) {
            case TOKEN_IDENTIFIER: //identifier
                break;
            case TOKEN_NUMBER: //number
                break;
            case TOKEN_OPERATOR: //operator
                break;
            case TOKEN_KEYWORD: //keywords
                switch (curToken->value[0]) { //all keywords have unique first char
                    case 'v': //var
                        nodeType = NODE_VAR_DECL;
                        subNode = createVarDecl(curToken);
                        curToken = jumpToNext(curToken);
                        break;
                    case 'f': //func
                        nodeType = NODE_FUNC;
                        //subNode = createFuncNode(curToken);
                        break;
                    case 'w': //while
                        nodeType = NODE_WHILE;
                        //subNode = createWhileNode(curToken);
                        break;
                    case 'i': //if
                        nodeType = NODE_IF;
                        //subNode = createIfNode(curToken);
                        break;
                    case 'e': //else
                        //look for preceding if
                        break;
                    case 'r': //return
                        //look if embedded in func and look for valid return val
                        break;
                    case 'c': //continue
                        //look if in while
                        break;
                    case 'b': //break
                        //look if in while
                        break;
                    default:
                        printf("unknown keyword %s", curToken->value);
                        return NULL;
                }
                break;
            case TOKEN_SEPERATOR: //seperators
                break;
            default:
                printf("Unknown token type %d", curToken->tokenType);
                return NULL;
        }
        //manage overarching linked list
        if (subNode == NULL)
            return NULL;
        ASTNode* newNode = createNode(nodeType, subNode, prevNode);
        if (headNode == NULL)
            headNode = newNode;
        prevNode = newNode;
        //increment
        curToken = curToken->nextToken;
    }
    return headNode;
}