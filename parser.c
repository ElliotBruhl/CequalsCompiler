#include "parser.h"
#include "tokenizer.h"
#include "identifierHashTable.h"


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

VarDeclNode* createVarDecl(char* varName, OperatorType opType, Token* tokens) {
    VarDeclNode* node = NULL;
    //check for valid declaration
    Token* curTok = tokens->nextToken;
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
    if ((curTok->tokenType == TOKEN_IDENTIFIER || curTok->tokenType == TOKEN_NUMBER) && curTok->nextToken != NULL //number or identifier followed by ';'
            && curTok->nextToken->tokenType == TOKEN_SEPERATOR && curTok->nextToken->value[0] == ';') {
        node = (VarDeclNode*)malloc(sizeof(VarDeclNode));
        if (node == NULL) {
            printf("malloc failed in creatVarDecl");
            return NULL;
        }
        node->isToken = true;
        node->value.token = curTok;
        node->nameHash = hash(tokens->nextToken->value);
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

ASTNode* parseTokens(Token* head) {
    Token* curToken = head;

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

                        break;
                    case 'f': //func
                        break;
                    case 'w': //while
                        break;
                    case 'i': //if
                        break;
                    case 'e': //else
                        break;
                    case 'r': //return
                        break;
                    case 'c': //continue
                        break;
                    case 'b': //break
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
    }
}