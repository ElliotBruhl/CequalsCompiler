#include "parser.h"
#include <stdlib.h>
#include <string.h>

void freeASTNodes(ASTNode* head) {
    //...
}
void printASTs(ASTNode* head) {
    //...
}

//HELPER FUNCTIONS FOR BUILDING POSTFIX EXPRESSION
typedef struct queueOrStackNode {
    ValueNode* value;
    struct queueOrStackNode* next;
} queueOrStackNode;
void printPostfix(queueOrStackNode* head) {
    printf("Postfix: ");
    if (head == NULL) return;
    queueOrStackNode* current = head;
    while (current != NULL) {
        switch (current->value->valueType) {
            case VALUE_OP:
                printf("OP: %d ", *(OperatorType*)current->value->value);
                break;
            case VALUE_NUM:
                printf("NUM: %lld ", *(long long*)current->value->value);
                break;
            case VALUE_VAR:
                printf("VAR: %s ", *(char*)current->value->value);
                break;
            case VALUE_FUNC_RET:
                printf("FUNC: %s ", *(char*)current->value->value);
                break;
            default:
                printf("Unknown value type in postfix print.\n");
                return;
        }
        current = current->next;
    }
    printf("\n");
}
void freePostfix(queueOrStackNode* head) {
    if (head == NULL) return;
    queueOrStackNode* current = head;
    while (current != NULL) {
        queueOrStackNode* next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}
int numParams(Token* head) { //takes in first paren and goes until matching end paren. -2 is error state
    if (head->nextToken->value[0] == ')') return 0;
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
                if (parens == 1) params++;
                break;
            case ';':
                return -2; //so invalid syntax doesnt result in read until end of file
        }
    }
    return parens == 0 ? params : -2;
}
OperatorType getOperatorType(Token* token, SymbolTable* table) { //-3 for func, -2 for var, -1 for number, 0 for error, positive for valid operator (do op/4 for precedence)
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
queueOrStackNode* enqueue(queueOrStackNode* back, ValueNode* value) { //returns new back
    queueOrStackNode* newNode = (queueOrStackNode*)malloc(sizeof(queueOrStackNode));
    if (newNode == NULL) {
        printf("\033[1;31mMalloc error in enqueue.\033[0m\n");
        return NULL;
    }
    newNode->value = value;
    newNode->next = NULL;
    if (back != NULL) back->next = newNode;
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
queueOrStackNode* push(queueOrStackNode* front, ValueNode* value) { //return new front (of stack)
    queueOrStackNode* newNode = (queueOrStackNode*)malloc(sizeof(queueOrStackNode));
    if (newNode == NULL) {
        printf("\033[1;31mMalloc error in push.\033[0m\n");
        return NULL;
    }
    newNode->value = value;
    newNode->next = front;
    return newNode;
}
queueOrStackNode* buildPostfix(Token* head, Token* endTok, SymbolTable* table) { //takes in math expression (terminated correctly with end token)
    if (head == NULL || endTok == NULL) {
        printf("\033[1;31mInvalid math operation.\033[0m\n");
        return NULL;
    }
    queueOrStackNode* outQ = NULL; //queue here
    queueOrStackNode* outQBack = NULL; //queue here
    queueOrStackNode* opStack = NULL; //stack here

    for (Token* current = head; current != endTok; current = current->nextToken) {
        OperatorType opType = getOperatorType(current, table);
        printf("opType %d\n", opType);
        if (opType == 0) {
            printf("\033[1;31mInvalid token or undefined identifier in math operation. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
            return NULL;
        }
        if (opType == -1) { //number -- add to ouput queue
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_NUM;
            newValNode->value = malloc(sizeof(long long));
            if (newValNode->value != NULL) {
                *(long long*)newValNode->value = atoll(current->value);
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            outQBack = enqueue(outQBack, newValNode);
            if (outQBack == NULL) {
                printf("\033[1;31mEnqueue Failed.\033[0m\n");
                return NULL;
            }
            if (outQ == NULL) outQ = outQBack;
        }
        else if (opType == -2) {//var -- add to output queue
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_VAR;
            newValNode->value = malloc(strlen(current->value) + 1);
            if (newValNode->value != NULL) {
                strcpy((char*)newValNode->value, current->value);
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            outQBack = enqueue(outQBack, newValNode);
            if (outQBack == NULL) {
                printf("\033[1;31mEnqueue Failed.\033[0m\n");
                return NULL;
            }
            if (outQ == NULL) outQ = outQBack;
        }
        else if (opType == -3) { //func -- add to output queue and skip to end of func call
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_FUNC_RET;
            newValNode->value = malloc(strlen(current->value) + 1);
            if (newValNode->value != NULL) {
                strcpy((char*)newValNode->value, current->value);
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            outQBack = enqueue(outQBack, newValNode);
            if (outQBack == NULL) {
                printf("\033[1;31mEnqueue Failed.\033[0m\n");
                return NULL;
            }
            if (outQ == NULL) outQ = outQBack;
            int parens = 1;
            for (current = current->nextToken->nextToken; current != endTok && parens > 0; current = current->nextToken) {
                if (current->value[0] == '(') parens++;
                if (current->value[0] == ')') parens--;
            }
        }
        else if (opType == OP_OPEN_PAREN) { //push onto stack
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_OP;
            newValNode->value = malloc(sizeof(OperatorType));
            if (newValNode->value != NULL) {
                *(OperatorType*)newValNode->value = opType;
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            opStack = push(opStack, newValNode);
            if (opStack == NULL) {
                printf("\033[1;31mPush failed.\033[0m\n");
                return NULL;
            }
        }
        else if (opType == OP_CLOSE_PAREN) { //pop stack until open paren
            if (opStack == NULL) {
                printf("\033[1;31mMismatched parentheses in math operation. Line %d.\033[0m\n", current->lineNum);
                return NULL;
            }
            while (*(OperatorType*)opStack->value->value != OP_OPEN_PAREN) {
                outQBack = enqueue(outQBack, opStack->value);
                if (outQBack == NULL) {
                    printf("\033[1;31mEnqueue Failed.\033[0m\n");
                    return NULL;
                }
                if (outQ == NULL) outQ = outQBack;
                opStack = dequeue(opStack);
                if (opStack == NULL) {
                    printf("\033[1;31mMismatched parentheses in math operation. Line %d.\033[0m\n", current->lineNum);
                    return NULL;
                }
            }
            opStack = dequeue(opStack); //pop open paren
        }
        else if (opType/4 == 1) { //unary op (r->l) -- push onto stack (in correct order)
            while (opStack != NULL && *(OperatorType*)opStack->value->value/4 < opType/4) {
                outQBack = enqueue(outQBack, opStack->value);
                if (outQBack == NULL) {
                    printf("\033[1;31mEnqueue Failed.\033[0m\n");
                    return NULL;
                }
                if (outQ == NULL) outQ = outQBack;
                opStack = dequeue(opStack);
            }
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_OP;
            newValNode->value = malloc(sizeof(OperatorType));
            if (newValNode->value != NULL) {
                *(OperatorType*)newValNode->value = opType;
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            opStack = push(opStack, newValNode);
            if (opStack == NULL) {
                printf("\033[1;31mPush failed.\033[0m\n");
                return NULL;
            }
        }
        else { //binary op (l->r) -- push onto stack (in correct order)
            while (opStack != NULL && *(OperatorType*)opStack->value->value/4 < opType/4) {
                outQBack = enqueue(outQBack, opStack->value);
                if (outQBack == NULL) {
                    printf("\033[1;31mEnqueue Failed.\033[0m\n");
                    return NULL;
                }
                if (outQ == NULL) outQ = outQBack;
                opStack = dequeue(opStack);
            }
            ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
            if (newValNode == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            newValNode->valueType = VALUE_OP;
            newValNode->value = malloc(sizeof(OperatorType));
            if (newValNode->value != NULL) {
                *(OperatorType*)newValNode->value = opType;
            }
            else {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            opStack = push(opStack, newValNode);
            if (opStack == NULL) {
                printf("\033[1;31mPush failed.\033[0m\n");
                return NULL;
            }
        }
    }
    while (opStack != NULL) {
        outQBack = enqueue(outQBack, opStack->value);
        if (outQBack == NULL) {
            printf("\033[1;31mEnqueue Failed.\033[0m\n");
            return NULL;
        }
        opStack = dequeue(opStack);
    }
    free(opStack);

    return outQ; //return head of postfix expression
}

MathOpNode* parseMathOp(Token* head, SymbolTable* table, int length) {
    if (head == NULL) return NULL;
    Token* current = head;
    for (int i = 0; i < length; i++) {
        if (current == NULL) {
            printf("\033[1;31mInvalid math operation length.\033[0m\n");
            return NULL;
        }
        current = current->nextToken;
    }
    queueOrStackNode* postfix = buildPostfix(head, current, table); //should be no reference to a token or token value in output (deep copied)
    if (postfix == NULL) {
        printf("\033[1;31mError building postfix expression.\033[0m\n");
        return NULL;
    }
    printPostfix(postfix);
    freePostfix(postfix);
    return NULL;
}
ASTNode* parseTokens(Token* head) {
    //...
}