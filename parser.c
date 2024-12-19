#include "parser.h"
#include <stdlib.h>
#include <string.h>

void freeASTNodes(ASTNode* head) { //TODO when otherwise finished
    //...
}
void printASTs(ASTNode* head) { //TODO when otherwise finished
    //...
}
//PARSING FUNCTION ARGUMENTS
FuncCallNode** parseFuncArgs(Token* head, VarTable* varTable, FuncTable* funcTable, int numArgs) { //takes in first paren and returns array of mathOpNodes
    if (head == NULL || varTable == NULL || funcTable == NULL || numArgs <= 0) {
        printf("\033[1;31mBad parameter to parseFuncArgs.\033[0m\n");
        return NULL;
    }
    if (head->value[0] != '(') {
        printf("\033[1;31mBad start token to parseFuncArgs.\033[0m\n");
        return NULL;
    }
    FuncCallNode** args = (FuncCallNode**)malloc(numArgs * sizeof(FuncCallNode*));
    if (args == NULL) {
        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
        return NULL;
    }
    
    if (numArgs == 1) {
        //parse the argument
    }
    else {
        for (int i = 0; i < numArgs; i++) {
            //parse each argument (split at commas)
        }
    }

    return args;
}
//PARSING MATH OPERATIONS
typedef struct queueOrStackNode {
    ValueNode* value;
    struct queueOrStackNode* next;
} queueOrStackNode;
void printPostfix(queueOrStackNode* head) { //DEBUG (temp)
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
        free(current->value->value); //void*
        free(current->value); //ValueNode*
        free(current); //queueOrStackNode*
        current = next;
    }
}
int numParams(Token* head) { //takes in first paren and goes until matching end paren. -1 is error state
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
                return -1; //so invalid syntax doesnt result in read until end of file
        }
    }
    return parens == 0 ? params : -1;
}
OperatorType getOperatorType(Token* token, VarTable* varTable, FuncTable* funcTable) { //-3 for func, -2 for var, -1 for number, 0 for error, positive for valid operator (do op/4 for precedence)
    if (token == NULL) {
        printf("\033[1;31mNull token in getOperatorType.\033[0m\n");
        return 0;
    }
    switch (token->tokenType) {
        case 0: //identifier (var/func)
            if (token->nextToken->value[0] == '(') //func
                if (funcLookup(funcTable, token->value, numParams(token->nextToken)) != NULL) return -3; //is func defined?
                printf("\033[1;31mUndefined function in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                return 0;
            if (varLookup(varTable, token->value) != NULL) return -2; //is var defined?
            printf("\033[1;31mUndefined variable in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return 0;
        case 1: //number
            return -1;
        case 2: //operator
            switch (token->value[0]) {
                case '=': 
                    if (token->value[1] == '=') return OP_EQU; // "=="
                    else {
                        printf("\033[1;31mInvalid operator = in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                        return 0; // "=" (error)
                    }
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
                default: 
                    printf("\033[1;31mUnknown operator in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                    return 0; //unknown operator (error)
            }
        case 3: 
            printf("\033[1;31mKeyword in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return 0; //keyword (error)
        case 4:
            if (token->value[0] == '(') return OP_OPEN_PAREN;
            if (token->value[0] == ')') return OP_CLOSE_PAREN;
            printf("\033[1;31mBad seperator in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return 0; //other seperator (error)
        default:
            printf("\033[1;31mUnknown token type in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
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
queueOrStackNode* buildPostfix(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //takes in math expression (inclusive) and returns postfix expression
    if (head == NULL || endTok == NULL) {
        printf("\033[1;31mNull parameter to buildPostfix.\033[0m\n");
        return NULL;
    }
    queueOrStackNode* outQ = NULL; //queue here
    queueOrStackNode* outQBack = NULL; //queue here
    queueOrStackNode* opStack = NULL; //stack here

    for (Token* current = head; current != endTok->nextToken; current = current->nextToken) {
        OperatorType opType = getOperatorType(current, varTable, funcTable);
        if (opType == 0) return NULL; //error
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
            int params = numParams(current->nextToken);
            if (params == -1) {
                printf("\033[1;31mInvalid function call in buildPostfix from numParams. Line %d.\033[0m\n", current->lineNum);
                return NULL;
            }
            newValNode->valueType = VALUE_FUNC_RET;
            newValNode->value = (FuncCallNode*)malloc(sizeof(FuncCallNode));
            if (newValNode->value == NULL) {
                printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                return NULL;
            }
            else { //create function call node
                strcpy((char*)((FuncCallNode*)newValNode->value)->funcName, current->value);
                ((FuncCallNode*)newValNode->value)->argCount = params;
                if (params == 0) ((FuncCallNode*)newValNode->value)->args = NULL;
                else {
                    ((FuncCallNode*)newValNode->value)->args = (ValueNode**)malloc(params * sizeof(ValueNode*));
                    if (((FuncCallNode*)newValNode->value)->args == NULL) { 
                        printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                        return NULL;
                    }
                    else { //parse function arguments
                        ((FuncCallNode*)newValNode->value)->args = parseFuncArgs(current->nextToken, varTable, funcTable, params);
                        if (((FuncCallNode*)newValNode->value)->args == NULL) {
                            printf("\033[1;31mError parsing function arguments in buildPostfix. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                    }
                }
            }
            outQBack = enqueue(outQBack, newValNode);
            if (outQBack == NULL) {
                printf("\033[1;31mEnqueue Failed.\033[0m\n");
                return NULL;
            }
            if (outQ == NULL) outQ = outQBack;
            int parens = 1;
            for (current = current->nextToken->nextToken; current != endTok && parens > 0; current = current->nextToken) { //skip to end of func call
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
            while (opStack != NULL && *(OperatorType*)opStack->value->value/4 != 0 && *(OperatorType*)opStack->value->value/4 < opType/4) {
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
            while (opStack != NULL && *(OperatorType*)opStack->value->value/4 != 0 && *(OperatorType*)opStack->value->value/4 <= opType/4) {
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
bool isValidMathOp(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //checks for valid math operation
    //check for null parameters
    if (head == NULL || endTok == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to isValidMathOp.\033[0m\n");
        return false;
    }
    //check for mismatched parentheses
    int parens = 0; 
    for (Token* current = head; current != endTok->nextToken; current = current->nextToken) {
        if (current->value[0] == '(') parens++;
        if (current->value[0] == ')') parens--;
    }
    if (parens != 0) {
        printf("\033[1;31mMismatched parentheses in math operation (isValidMathOp) Line %d.\033[0m\n", endTok->lineNum);
        return false;
    }
    //check start and end tokens
    OperatorType currentTokenType = getOperatorType(endTok, varTable, funcTable); 
    if (currentTokenType >= 0 && currentTokenType != OP_CLOSE_PAREN) { //can't end with operator or open paren
        printf("\033[1;31mExpression ends with operator or open paren (isValidMathOp). Line %d. Value %s.\033[0m\n", endTok->lineNum, endTok->value);
        return false;
    }
    currentTokenType = getOperatorType(head, varTable, funcTable);
    if (currentTokenType == NULL_OP || currentTokenType == OP_CLOSE_PAREN || currentTokenType >= OP_MUL) { //can't start with binary op or close paren
        printf("\033[1;31mExpression starts with a binary op or close paren (isValidMathOp). Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return false;
    }
    OperatorType previousTokenType = currentTokenType;
    //check sequence of tokens
    for (Token* current = head->nextToken; current != endTok; current = current->nextToken) {
        currentTokenType = getOperatorType(current, varTable, funcTable);
        if (currentTokenType == NULL_OP) { //invalid token
            printf("\033[1;31mInvalid token in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
            return false; 
        }
        if (currentTokenType < 0) { //operand
            if (previousTokenType < 0) {
                printf("\033[1;31mSequential operands in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
            if (previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mOperand following close paren in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType == OP_OPEN_PAREN) { //open parenthesis
            if (previousTokenType < 0) {
                printf("\033[1;31mOpen paren following operand in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
            if (previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mOpen paren following close paren in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType == OP_CLOSE_PAREN) { //close parenthesis
            if (previousTokenType >= 0) {
                printf("\033[1;31mClose paren following operator or start of math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_BIT_NOT && currentTokenType <= OP_DEREF) { //unary operator
            if (previousTokenType < 0 || previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mUnary operator following operand or close parenthesis in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_MUL) { //binary operator
            if (previousTokenType >= 0 && previousTokenType != OP_CLOSE_PAREN) {
                printf("\033[1;31mBinary operator following another operator or open parenthesis (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        previousTokenType = currentTokenType;
        if (currentTokenType == -3) { //skip to end of func call
            int parens = 1;
            for (current = current->nextToken->nextToken; current != endTok && parens > 0; current = current->nextToken) {
                if (current->value[0] == '(') parens++;
                if (current->value[0] == ')') parens--;
            }
        }
    }
    return true;
}
MathOpNode* parseMathOp(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //takes in math expression (inclusive) and returns AST
    if (head == NULL || endTok == NULL || varTable == NULL || funcTable == NULL) { //check for null
        printf("\033[1;31mNull parameter to parseMathOp.\033[0m\n");
        return NULL;
    }
    if (head == endTok) { //check for empty
        printf("\033[1;31mEmpty math operation to parseMathOp.\033[0m\n");
        return NULL;
    }
    Token* current = head;
    while (current != endTok) { //check for tail
        if (current == NULL) {
            printf("\033[1;31mHead token doesn't lead to tail in parseMathOp.\033[0m\n");
            return NULL;
        }
        current = current->nextToken;
    }
    if (!isValidMathOp(head, endTok, varTable, funcTable)) { //check for valid token sequences
        printf("\033[1;31misValidMathOp failed.\033[0m\n");
        return NULL;
    }
    queueOrStackNode* postfix = buildPostfix(head, endTok, varTable, funcTable); //build postfix (deep copied values from tokens)
    if (postfix == NULL) {
        printf("\033[1;31mError building postfix expression.\033[0m\n");
        return NULL;
    }
    //TODO - build AST from postfix

    //DEBUG
    printPostfix(postfix);
    freePostfix(postfix);
    return NULL;
}

//MAIN PARSING FUNCTION
ASTNode* parseTokens(Token* head, VarTable* varTable, FuncTable* funcTable) { //takes in tokens and returns AST
    //...
}