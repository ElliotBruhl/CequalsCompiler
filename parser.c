#include "parser.h"
#include <stdlib.h>
#include <string.h>

//FORWARD DECLARATIONS
FuncCallNode* parseFuncArgs(Token* head, VarTable* varTable, FuncTable* funcTable, int numArgs);

//DEBUG FUNCTIONS
void printValueNode(ValueNode* value) { //DEBUG (temp)
    printf("Printing ValueNode:\n");
    if (value == NULL) {
        printf("NULL ValueNode\n");
        return;
    }
    switch (value->valueType) {
        case VALUE_OP:
            printf("OP: %d ", *(OperatorType*)value->value);
            break;
        case VALUE_NUM:
            printf("NUM: %lld ", *(long long*)value->value);
            break;
        case VALUE_VAR:
            printf("VAR: %s ", (char*)value->value);
            break;
        case VALUE_FUNC_RET:
            printf("FUNC: %s ", ((FuncCallNode*)value->value)->funcName);
            break;
        case VALUE_MATH_OP:
            if (((MathOpNode*)value->value)->left == NULL)
                printf("MathOp: Left Type: NULL Right Type: %d OP: %d", ((MathOpNode*)value->value)->right->valueType, ((MathOpNode*)value->value)->opType);
            else
                printf("MathOp: Left Type: %d Right Type: %d OP: %d", ((MathOpNode*)value->value)->left->valueType, ((MathOpNode*)value->value)->right->valueType, ((MathOpNode*)value->value)->opType);
            break;
        default:
            printf("Unknown value type in value print.\n");
    }
    printf("\n");
}
void printPostfix(queueOrStackNode* head) { //DEBUG (temp)
    printf("Printing Postfix:\n");
    if (head == NULL) {
        printf("NULL Postfix\n");
        return;
    }
    queueOrStackNode* current = head;
    while (current != NULL) {
        printValueNode(current->value);
        current = current->next;
    }
}
void printASTs(ASTNode* head) { //DEBUG (temp)
    printf("Printing ASTs:\n");
    if (head == NULL) {
        printf("NULL AST\n");
        return;
    }
    ASTNode* current = head;
    while (current != NULL) {
        switch (current->nodeType) {
            case NODE_VAR_DECL:
                printf("VarDecl: %s\n", ((VarDeclNode*)current->subNode)->varName);
                break;
            case NODE_VAR_ASSIGN:
                printf("VarAssign: %s -> ", ((VarAssignNode*)current->subNode)->varName);
                printValueNode(((VarAssignNode*)current->subNode)->newValue);
                printf("\n");
                break;
            case NODE_FUNC_DECL:
                printf("FuncDecl: %s (%d arg(s))\n", ((FuncDeclNode*)current->subNode)->funcName, ((FuncDeclNode*)current->subNode)->argCount);
                break;
            case NODE_WHILE:
                printf("While: ");
                printValueNode(((WhileNode*)current->subNode)->condition);
                printf("\n");
                break;
            case NODE_IF:
                printf("If: ");
                printValueNode(((IfNode*)current->subNode)->condition);
                printf(" Has else: %d\n", ((IfNode*)current->subNode)->elseBody != NULL);
                break;
            default:
                printf("Unknown node type in AST print.\n");
        }
        current = current->next;
    }
}
//FREE FUNCTIONS
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
void freeASTNodes(ASTNode* head) { //TODO
    //...
}
//HELPER FUNCTIONS
Token* skipFuncParams(Token* head) { //takes in first paren and returns matching end paren
    if (head == NULL || head->value == NULL) {
        printf("\033[1;31mNull parameter to skipFuncParams.\033[0m\n");
        return NULL;
    }
    if (head->value[0] != '(') {
        printf("\033[1;31mBad start token to skipFuncParams.\033[0m\n");
        return NULL;
    }
    int parens = 1;
    for (Token* current = head->nextToken; current != NULL && parens > 0; current = current->nextToken) {
        if (current->value[0] == '(') parens++;
        else if (current->value[0] == ')') parens--;
        if (parens == 0) return current;
    }
    return NULL;
}
int numParams(Token* head) { //takes in first paren and goes until matching end paren. -1 is error state
    if (head == NULL || head->nextToken == NULL) return -1;
    if (head->value[0] != '(') return -1;
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
                return -1;
        }
    }
    return parens == 0 ? params : -1;
}
OperatorType getOperatorType(Token* token, VarTable* varTable, FuncTable* funcTable) { //-3 for func, -2 for var, -1 for number, 0 for error, positive for valid operator (do op/4 for precedence)
    if (token == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to getOperatorType.\033[0m\n");
        return 0;
    }
    switch (token->tokenType) {
        case 0: //identifier (var/func)
            if (token->nextToken->value[0] == '(') { //func
                if (funcLookup(funcTable, token->value, numParams(token->nextToken)) != NULL) //is func defined?
                    return -3;
                else {
                    printf("\033[1;31mUndefined function in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                    return 0;
                }
            }
            else { //var
                if (varLookup(varTable, token->value) != NULL) //is var defined?
                    return -2;
                else {
                    printf("\033[1;31mUndefined variable in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                    return 0;  
                }
            }
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
//PARSING MATH OPERATIONS
    //POSTFIX CONVERSION
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

    for (Token* current = head; current != endTok->nextToken && current != NULL; current = current->nextToken) {
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
            if (params == 0) { //create the function call node here
                newValNode->value = (FuncCallNode*)malloc(sizeof(FuncCallNode));
                if (newValNode->value == NULL) {
                    printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                    return NULL;
                }
                ((FuncCallNode*)newValNode->value)->funcName = malloc(strlen(current->value) + 1);
                if (((FuncCallNode*)newValNode->value)->funcName == NULL) {
                    printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                    return NULL;
                }
                strcpy(((FuncCallNode*)newValNode->value)->funcName, current->value);
                ((FuncCallNode*)newValNode->value)->argCount = 0;
                ((FuncCallNode*)newValNode->value)->args = NULL;
            }
            else { //parse the function call with helper function
                newValNode->value = parseFuncArgs(current->nextToken, varTable, funcTable, params);
                if (newValNode->value == NULL) {
                    printf("\033[1;31mparseFuncArgs failed on function call in line %d.\033[0m\n", current->lineNum);
                    return NULL;
                }
            }
            outQBack = enqueue(outQBack, newValNode);
            if (outQBack == NULL) {
                printf("\033[1;31mEnqueue Failed.\033[0m\n");
                return NULL;
            }
            if (outQ == NULL) outQ = outQBack;
            current = skipFuncParams(current->nextToken);
            if (current == NULL) {
                printf("\033[1;31mError skipping function parameters in buildPostfix.\033[0m\n");
                return NULL;
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
    //POSTFIX TO TREE
ValueNode* deepCopyValueNode(ValueNode* old) {
    if (old->valueType == VALUE_MATH_OP) { //should already be deep copied
        return old;
    }
    if (old == NULL || old->valueType == VALUE_OP || old->value == NULL) {
        printf("\033[1;31mBad value node passed to deepCopyValueNode.\033[0m\n");
        return NULL;
    }
    ValueNode* new = (ValueNode*)malloc(sizeof(ValueNode));
    if (new == NULL) {
        printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
        return NULL;
    }
    new->valueType = old->valueType;
    switch (old->valueType) {
        case VALUE_NUM:
            new->value = malloc(sizeof(long long));
            if (new->value == NULL) {
                printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
                return NULL;
            }
            *(long long*)new->value = *(long long*)old->value;
            break;
        case VALUE_VAR:
            new->value = malloc(strlen((char*)old->value) + 1);
            if (new->value == NULL) {
                printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
                return NULL;
            }
            strcpy((char*)new->value, (char*)old->value);
            break;
        case VALUE_FUNC_RET:
            new->value = malloc(sizeof(FuncCallNode));
            if (new->value == NULL) {
                printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
                return NULL;
            }
            ((FuncCallNode*)new->value)->funcName = malloc(strlen(((FuncCallNode*)old->value)->funcName) + 1);
            if (((FuncCallNode*)new->value)->funcName == NULL) {
                printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
                return NULL;
            }
            strcpy(((FuncCallNode*)new->value)->funcName, ((FuncCallNode*)old->value)->funcName);
            ((FuncCallNode*)new->value)->argCount = ((FuncCallNode*)old->value)->argCount;
            if (((FuncCallNode*)old->value)->argCount == 0) {
                ((FuncCallNode*)new->value)->args = NULL;
            }
            else {
                ((FuncCallNode*)new->value)->args = malloc(sizeof(ValueNode*) * ((FuncCallNode*)old->value)->argCount);
                if (((FuncCallNode*)new->value)->args == NULL) {
                    printf("\033[1;31mMalloc error in deepCopyValueNode.\033[0m\n");
                    return NULL;
                }
                for (int i = 0; i < ((FuncCallNode*)old->value)->argCount; i++) {
                    ((FuncCallNode*)new->value)->args[i] = deepCopyValueNode(((FuncCallNode*)old->value)->args[i]); //error when passing math op
                    if (((FuncCallNode*)new->value)->args[i] == NULL) {
                        printf("\033[1;31mdeepCopyValueNode failed in deepCopyValueNode.\033[0m\n");
                        return NULL;
                    }
                }
            }
            break;
    }
    return new;       
}
MathOpNode* postfixToTree(queueOrStackNode* head) { //takes in postfix expression and returns binary tree
    if (head == NULL) {
        printf("\033[1;31mNull parameter to postfixToTree.\033[0m\n");
        return NULL;
    }
    queueOrStackNode* nodeStack = NULL;
    for (queueOrStackNode* current = head; current != NULL; current = current->next) {
        switch (current->value->valueType) {
            case VALUE_OP:
                //create new node
                MathOpNode* newNode = (MathOpNode*)malloc(sizeof(MathOpNode));
                if (newNode == NULL) {
                    printf("\033[1;31mMalloc error in postfixToTree.\033[0m\n");
                    return NULL;
                }
                newNode->opType = *(OperatorType*)current->value->value;
                if (nodeStack == NULL) {
                    printf("\033[1;31mStackunderflow in postfixToTree.\033[0m\n");
                    return NULL;
                }
                //get left and/or right children
                newNode->right = nodeStack->value;
                nodeStack = dequeue(nodeStack);
                if (newNode->opType >= OP_MUL) { //for binary operators
                    if (nodeStack == NULL) {
                        printf("\033[1;31mStackunderflow in postfixToTree.\033[0m\n");
                        return NULL;
                    }
                    newNode->left = nodeStack->value;
                    nodeStack = dequeue(nodeStack);
                }
                else newNode->left = NULL; //for unary operators
                //wrap as value node and push back to stack
                ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
                if (newValNode == NULL) {
                    printf("\033[1;31mMalloc error in postfixToTree.\033[0m\n");
                    return NULL;
                }
                newValNode->valueType = VALUE_MATH_OP;
                newValNode->value = newNode;
                nodeStack = push(nodeStack, newValNode);
                break;
            case VALUE_NUM:
            case VALUE_VAR:
            case VALUE_FUNC_RET:
                ValueNode* newValNodeCopy = deepCopyValueNode(current->value); //deepcopy func ret fails
                if (newValNodeCopy == NULL) {
                    printf("\033[1;31mdeepCopyValueNode failed in postfixToTree.\033[0m\n");
                    return NULL;
                }
                nodeStack = push(nodeStack, newValNodeCopy);
                break;
            case VALUE_MATH_OP:
                printf("\033[1;31mMathOpNode in postfixToTree.\033[0m\n");
                return NULL; //shouldn't be here
        }
    }
    if (nodeStack == NULL || nodeStack->next != NULL) { //must be only one node left
        printf("\033[1;31mNot one node left when returning tree in postfixToTree.\033[0m\n");
        return NULL;
    }
    if (nodeStack->value->valueType != VALUE_MATH_OP) {
        printf("\033[1;31mRoot node is not math op node in postfixToTree.\033[0m\n");
        return NULL;
    }

    freePostfix(head);

    return nodeStack->value->value;
}
    //PARSE FUNCTION ARGUMENTS
int getFuncArgType(Token* head, Token* tail) { //head is first token of arg and tail is last token of arg (neither should be a parenthesis). -1 error, 0 simple arg, 1 func call, 2 math op
    if (head == NULL || tail == NULL) {
        printf("\033[1;31mNull parameter to getFuncArgType.\033[0m\n");
        return -1;
    }
    if (head == tail) { //1 token argument (simple arg)
        return 0;
    }
    if (head->tokenType == TOKEN_IDENTIFIER && head->nextToken->value[0] == '(') {
        //go to end of call to determine if function call or math op that starts with func call
        Token* endOfCall = skipFuncParams(head->nextToken);
        if (endOfCall == NULL) return -1; //error
        if (endOfCall == tail) return 1; //function call
    }
    return 2; //math op
}
FuncCallNode* parseFuncArgs(Token* head, VarTable* varTable, FuncTable* funcTable, int numArgs) { //takes in first paren of call and returns call formatted as func call node
    if (head == NULL || varTable == NULL || funcTable == NULL || numArgs <= 0) {
        printf("\033[1;31mBad parameter to parseFuncArgs.\033[0m\n");
        return NULL;
    }
    if (head->value[0] != '(') {
        printf("\033[1;31mBad start token to parseFuncArgs.\033[0m\n");
        return NULL;
    }
    FuncCallNode* funcCall = (FuncCallNode*)malloc(sizeof(FuncCallNode));
    if (funcCall == NULL) {
        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
        return NULL;
    }
    //copy prev token value to funcCall->funcName
    funcCall->funcName = (char*)malloc(strlen(head->prevToken->value) + 1);
    if (funcCall->funcName == NULL) {
        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
        return NULL;
    }
    strcpy(funcCall->funcName, head->prevToken->value);
    funcCall->argCount = numArgs;
    funcCall->args = (ValueNode**)malloc(numArgs * sizeof(ValueNode*));
    if (funcCall->args == NULL) {
        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
        return NULL;
    }
    for (int i = 0; i < numArgs; i++) {
        funcCall->args[i] = (ValueNode*)malloc(sizeof(ValueNode));
        if (funcCall->args[i] == NULL) {
            printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
            return NULL;
        }
    }
    Token* startToken = head->nextToken;
    for (int i = 0; i < numArgs; i++) { //each iteration starts on first token of the argument
        //seperate the argument
        if (startToken == NULL) {
            printf("\033[1;31mUnexpected end of file in parseFuncArgs.\033[0m\n");
            return NULL;
        }
        int parens = 1;
        Token* endToken = startToken;
        while (true) { //move end to last token of argument - start and end should be exclusive
            if (endToken == NULL || endToken->value[0] == ';') {
                printf("\033[1;31mError within argument in parseFuncArgs.\033[0m\n");
                return NULL;
            }
            if (endToken->value[0] == '(') parens++;
            if (endToken->value[0] == ')')
                if (--parens == 0) break;
            if (endToken->value[0] == ',' && parens == 1) break;
            endToken = endToken->nextToken;
        }
        //parse the argument
        int argType = getFuncArgType(startToken, endToken->prevToken); 
        switch (argType) {
            case 0: //simple argument
                if (funcCall->args[i] == NULL) {
                    printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                if (startToken->tokenType == TOKEN_IDENTIFIER) {
                    if (varLookup(varTable, startToken->value) == NULL) {
                        printf("\033[1;31mUndefined variable in function argument in parseFuncArgs.\033[0m\n");
                        return NULL;
                    }
                    funcCall->args[i]->valueType = VALUE_VAR;
                    funcCall->args[i]->value = malloc(strlen(startToken->value) + 1);
                    if (funcCall->args[i]->value == NULL) {
                        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                        return NULL;
                    }
                    strcpy((char*)funcCall->args[i]->value, startToken->value);
                }
                else if (startToken->tokenType == TOKEN_NUMBER) {
                    funcCall->args[i]->valueType = VALUE_NUM;
                    funcCall->args[i]->value = malloc(sizeof(long long));
                    if (funcCall->args[i]->value == NULL) {
                        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                        return NULL;
                    }
                    *(long long*)funcCall->args[i]->value = atoll(startToken->value);
                }
                else {
                    printf("\033[1;31mBad argument type from getFuncArgType in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                break;
            case 1: //function call
                if (funcCall->args[i] == NULL) {
                    printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                int innerParams = numParams(startToken->nextToken);
                if (innerParams == -1) {
                    printf("\033[1;31mInvalid inner function call in parseFuncArgs from numParams. Line %d.\033[0m\n", startToken->lineNum);
                    return NULL;
                }
                if (funcLookup(funcTable, startToken->value, innerParams) == NULL) {
                    printf("\033[1;31mUndefined function in function argument in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                funcCall->args[i]->valueType = VALUE_FUNC_RET;
                funcCall->args[i]->value = (FuncCallNode*)malloc(sizeof(FuncCallNode));
                if (funcCall->args[i]->value == NULL) {
                    printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                if (innerParams == -1) {
                    printf("\033[1;31mError parsing nested function argument in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                if (innerParams == 0) {
                    ((FuncCallNode*)(funcCall->args[i]->value))->funcName = (char*)malloc(strlen(startToken->prevToken->value) + 1);
                    if (((FuncCallNode*)(funcCall->args[i]->value))->funcName == NULL) {
                        printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                        return NULL;
                    }
                    strcpy(((FuncCallNode*)(funcCall->args[i]->value))->funcName, startToken->prevToken->value);
                    ((FuncCallNode*)(funcCall->args[i]->value))->argCount = 0;
                    ((FuncCallNode*)(funcCall->args[i]->value))->args = NULL;
                }
                else {
                    funcCall->args[i]->value = parseFuncArgs(startToken->nextToken, varTable, funcTable, innerParams); //causes errors -------------------------------------------------------------------------
                    if (funcCall->args[i]->value == NULL) {
                        printf("\033[1;31mError parsing nested function argument in parseFuncArgs.\033[0m\n");
                        return NULL;
                    }
                }
                break;
            case 2: //math operation
                if (funcCall->args[i] == NULL) {
                    printf("\033[1;31mMalloc error in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                funcCall->args[i]->valueType = VALUE_MATH_OP;
                funcCall->args[i]->value = parseMathOp(startToken, endToken->prevToken, varTable, funcTable);
                if (funcCall->args[i]->value == NULL) {
                    printf("\033[1;31mError parsing math operation in function argument in parseFuncArgs.\033[0m\n");
                    return NULL;
                }
                break;
            default: //error
                printf("\033[1;31mBad argument type from getFuncArgType in parseFuncArgs.\033[0m\n");
                return NULL;
        }

        startToken = endToken->nextToken;
    }
    return funcCall;
}
    //VALIDATION/MAIN FUNCTIONS
bool isValidMathOp(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //checks for valid math operation (inclusive)
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
    if (currentTokenType == 0) {
        printf("\033[1;31mError token encountered in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", endTok->lineNum, endTok->value);
        return false;
    }
    if (currentTokenType > 0 && currentTokenType != OP_CLOSE_PAREN) { //can't end with operator or open paren
        printf("\033[1;31mExpression ends with operator or open paren (isValidMathOp). Line %d. Value %s.\033[0m\n", endTok->lineNum, endTok->value);
        return false;
    }
    currentTokenType = getOperatorType(head, varTable, funcTable);
    if (currentTokenType == 0) {
        printf("\033[1;31mError token encountered math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return false;
    }
    if (currentTokenType == OP_CLOSE_PAREN || currentTokenType >= OP_MUL) { //can't start with binary op or close paren
        printf("\033[1;31mExpression starts with a binary op or close paren (isValidMathOp). Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return false;
    }
    if (currentTokenType == -3) {//skip to end of func call
        head = skipFuncParams(head->nextToken);
        if (head == NULL) {
            printf("\033[1;31mError skipping function parameters in buildPostfix.\033[0m\n");
            return false;
        }
        if (head == endTok) {
            printf("\033[1;31mSimple func call not mathOp (isValidMathOp). Line %d.\033[0m\n", head->lineNum);
            return false;
        }
    }
    OperatorType previousTokenType = currentTokenType;
    //check sequence of tokens
    Token* current = head->nextToken;
    while (current != endTok) {
        currentTokenType = getOperatorType(current, varTable, funcTable);
        if (currentTokenType == 0) { //invalid token
            printf("\033[1;31mError token encountered in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
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
            if (previousTokenType > 0) {
                printf("\033[1;31mClose paren following operator (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_BIT_NOT && currentTokenType <= OP_DEREF) { //unary operator
            if (previousTokenType < 0 || previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mUnary operator following operand or close parenthesis (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_MUL) { //binary operator
            if (previousTokenType > 0 && previousTokenType != OP_CLOSE_PAREN) {
                printf("\033[1;31mBinary operator following another operator or open parenthesis (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        if (currentTokenType == -3) { //skip to end of func call
            current = skipFuncParams(current->nextToken);
            if (current == NULL) {
                printf("\033[1;31mError skipping function parameters in buildPostfix.\033[0m\n");
                return NULL;
            }
            if (current == endTok) break; //shouldn't this line be irrelevant because of for loop condition? (current != endTok)
        }
        current = current->nextToken;
        previousTokenType = currentTokenType;
    }
    return true;
}
MathOpNode* parseMathOp(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //takes in math expression (inclusive) and returns AST
    if (head == NULL || endTok == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseMathOp.\033[0m\n");
        return NULL;
    }
    if (head == endTok) { //check for empty
        printf("\033[1;31mOne token math operation passed to parseMathOp. Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return NULL;
    }
    Token* current = head;
    while (current != endTok) { //check for tail
        if (current == NULL) {
            printf("\033[1;31mHead token doesn't lead to tail in parseMathOp. Line %d.\033[0m\n", head->lineNum);
            return NULL;
        }
        current = current->nextToken;
    }
    if (!isValidMathOp(head, endTok, varTable, funcTable)) { //check for valid token sequences
        printf("\033[1;31mError when validating math operation. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }
    queueOrStackNode* postfix = buildPostfix(head, endTok, varTable, funcTable); //build postfix (deep copied values from tokens)
    if (postfix == NULL) {
        printf("\033[1;31mError building postfix expression. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }

    MathOpNode* mathOpTree = postfixToTree(postfix); //build binary tree from postfix
    if (mathOpTree == NULL) {
        printf("\033[1;31mError building AST from postfix expression. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }

    return mathOpTree;
}

//MAIN PARSING FUNCTION
ASTNode* parseTokens(Token* head, VarTable* varTable, FuncTable* funcTable) { //takes in tokens and returns AST
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseTokens.\033[0m\n");
        return NULL;
    }
    ASTNode* prevNode = NULL;
    for (Token* current = head; current != NULL; current = current->nextToken) {
        ASTNode* newASTNode = (ASTNode*)malloc(sizeof(ASTNode));
        if (newASTNode == NULL) {
            printf("\033[1;31mMalloc error in parseTokens.\033[0m\n");
            return NULL;
        }
        newASTNode->prev = prevNode;
        newASTNode->next = NULL;

        switch (current->tokenType) {
            case TOKEN_IDENTIFIER: //start of a var assignment
                newASTNode->nodeType = NODE_VAR_ASSIGN;
                newASTNode->subNode = parseVarAssign(current, varTable, funcTable);
                if (newASTNode->subNode == NULL) {
                    printf("\033[1;31mError parsing variable assignment. Line %d.\033[0m\n", current->lineNum);
                    return NULL;
                }
                break;
            case TOKEN_KEYWORD:
                switch(current->value[0]) {
                    case 'i': //if statement
                        newASTNode->nodeType = NODE_IF;
                        newASTNode->subNode = parseIf(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing if statement. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        break;
                    case 'w': //while loop
                        newASTNode->nodeType = NODE_WHILE;
                        newASTNode->subNode = parseWhile(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing while loop. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        break;
                    case 'f': //function definition
                        newASTNode->nodeType = NODE_FUNC_DECL;
                        newASTNode->subNode = parseFuncDef(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing function definition. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        break;
                    case 'r': //return statement
                        //...
                        break;
                    case 'c':
                        //...
                        break;
                    case 'b':
                        //...
                        break;
                    case 'e':
                        //...
                        break;
                    default:
                        printf("\033[1;31mIllegal keyword in parseTokens. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                }
            default:
                printf("\033[1;31mIllegal start of statement. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return NULL;
        }
        if (prevNode != NULL) prevNode->next = newASTNode;
        prevNode = newASTNode;
    }
    
}