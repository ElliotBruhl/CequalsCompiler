#include "parser.h"
#include "symbolTable.h"
#include <stdlib.h>

void freeASTNodes(ASTNode* head) {
    //...
}
void printASTs(ASTNode* head) {
    //...
}
int numParams(Token* head) { //takes in first paren and goes until matching end paren
    if (head->nextToken->value == ')') return 0;
    int parens = 1;
    int params = 1;
    for (Token* current = head->nextToken; current != NULL && parens > 0; current = current->nextToken) {
        switch (current->value[0]) {
            case '(':
                parens++;
                break;
            case ')':
                parens--;
                break;
            case ',':
                params++;
                break;
            case ';':
                return -1; //so invalid syntax doesnt result in read until end of file
        }
    }
    return params;
}
OperatorType getOperatorType(Token* token, SymbolTable* table) { //-3 for func, -2 for var, -1 for number, 0 for error, other for valid operator (do op/4 for precedence)
    switch (token->tokenType) {
        case 0: //var or func
            if (token->nextToken->value[0] == '(')
                return lookup(table, token->value, numParams(token->nextToken)) != NULL ? -3 : 0; //is func defined?
            return lookup(table, token->value, -1) != NULL ? -2 : 0; //is var defined?
        case 1: //number
            return -1;
        case 2: //operator
            switch (token->value[0]) {
                case '=': 
                    if (token->value[1] == '=') return OP_EQU; // "=="
                    else return 0; // "=" (error)
                case '~': return OP_BIT_NOT;
                case '!': 
                    if (token->value[1] == '=') return OP_NEQ; // "!="
                    else return OP_NOT; // "!"
                case '&': 
                    if (token->value[1] == '&') return OP_AND; // "&&"
                    if (token->value[1] == ' ') return OP_BIT_AND; // "& " (with a space)
                    else return OP_REF; // "&"
                case '*':
                    if (token->value[1] == ' ') return OP_MUL; // "* " (with a space)
                    else return OP_DEREF; // "*"
                case '/': return OP_DIV;
                case '%': return OP_MOD;
                case '+': return OP_ADD;
                case '-': return OP_SUB;
                case '<':
                    if (token->value[1] == '<') return OP_BIT_L; // "<<"
                    if (token->value[1] == '=') return OP_LTE; // "<="
                    else return OP_LT; // "<"
                case '>':
                    if (token->value[1] == '>') return OP_BIT_R; // ">>"
                    if (token->value[1] == '=') return OP_GTE; // ">="
                    else return OP_GT; // ">"
                case '^': return OP_BIT_XOR;
                case '|': 
                    if (token->value[1] == '|') return OP_OR; // "||"
                    else return OP_BIT_OR; // "|"
                default: return 0; //unknown operator (error)
            }
        case 3: return 0; //keyword (error)
        case 4:
            if (token->value[0] == '(') return OP_OPEN_PAREN;
            if (token->value[0] == ')') return OP_CLOSE_PAREN;
            return 0; //other seperator (error)
        default:
            return 0; //unknown token (error)
    }
}
typedef struct queueOrStackNode {
    ValueType* value;
    struct queueOrStackNode* next;
} queueOrStackNode;
queueOrStackNode* enqueue(queueOrStackNode* back, ValueType* value) { //returns new back
    queueOrStackNode* newNode = (queueOrStackNode*)malloc(sizeof(queueOrStackNode));
    if (newNode == NULL) {
        printf("\033[1;31mMalloc error in enqueue.\033[0m\n");
        return NULL;
    }
    newNode->value = value;
    newNode->next = NULL;
    back->next = newNode;
    return newNode;
}
queueOrStackNode* dequeue(queueOrStackNode* front) { //returns new front
    if (front->next == NULL) {
        return NULL;
    }
    queueOrStackNode* newFront = front->next;
    free(front);
    return newFront;
}
queueOrStackNode* push(queueOrStackNode* front, ValueType* value) { //return new front (of stack)
    queueOrStackNode* newNode = (queueOrStackNode*)malloc(sizeof(queueOrStackNode));
    if (newNode == NULL) {
        printf("\033[1;31mMalloc error in push.\033[0m\n");
        return NULL;
    }
    newNode->value = value;
    newNode->next = front;
    return newNode;
}

Token* buildPostfix(Token* head, Token* endTok, SymbolTable* table) { //takes in math expression (terminated correctly with end token)
    if (head == NULL || endTok == NULL) return NULL;
    queueOrStackNode* outQueueFront = NULL; //queue here
    queueOrStackNode* outQueueBack = NULL; //queue here
    queueOrStackNode* opStack = NULL; //stack here

    for (Token* current = head; current != endTok; current = current->nextToken) {
        OperatorType opType = getOperatorType(current, table);
        if (opType == 0) {
            printf("\033[1;31mInvalid token or undefined identifier in math operation. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
            return NULL;
        }
        if (opType == -1) { //number
            //add to output queue
        }
        else if (opType == -2) {//var
            //add to output queue
        }
        else if (opType == -3) { //func
            //add to output queue
        }
        else if (opType == OP_OPEN_PAREN) {
            //push onto stack
        }
        else if (opType == OP_CLOSE_PAREN) {
            /*
            - While the token at the top of `operatorStack` is not a left parenthesis:
            - Pop operators from `operatorStack` to `outputQueue`.
            - Pop the left parenthesis `(` from `operatorStack` (do not add it to `outputQueue`).
            */
        }
        else if (opType/4 == 1) { //unary op
            /*
            - While `operatorStack` is not empty and:
            - The token at the top of `operatorStack` has higher precedence, or
            - The token at the top has equal precedence and is left associative,
            - Pop operators from `operatorStack` to `outputQueue`.
            - Push the current unary operator onto `operatorStack`.
            */
        }
        else { //binary op
            /*
            - While `operatorStack` is not empty and:
            - The token at the top of `operatorStack` has higher precedence, or
            - The token at the top has equal precedence and is left associative,
            - Pop operators from `operatorStack` to `outputQueue`.
            - Push the current binary operator onto `operatorStack`.
            */
        }
    }
    /*
    After all tokens have been read:
    - While `operatorStack` is not empty:
    - Pop operators from `operatorStack` to `outputQueue`.
    */

    return outQueueFront; //return head of postfix expression
}

MathOpNode* parseMathOp(Token* head, SymbolTable* table, int length) {
    //...
}

ASTNode* parseTokens(Token* head) {
    //...
}