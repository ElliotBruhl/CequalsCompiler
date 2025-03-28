#include "parser.h"
#include <stdlib.h>
#include <string.h>

//FORWARD DECLARATIONS
FuncCallNode* parseFuncCall(Token* head, VarTable* varTable, FuncTable* funcTable, int numArgs);
ValueNode* parseValueNode(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable);
void freeValueNode(ValueNode* head);
//DEBUG FUNCTIONS
char* opToString(OperatorType op) {
    switch (op) {
        case NULL_OP: return "NULL_OP";
        case OP_OPEN_PAREN: return "(";
        case OP_CLOSE_PAREN: return ")";
        case OP_BIT_NOT: return "~";
        case OP_NOT: return "!";
        case OP_REF: return "&";
        case OP_DEREF: return "*";
        case OP_MUL: return "* ";
        case OP_DIV: return "/";
        case OP_MOD: return "%%";
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_BIT_L: return "<<";
        case OP_BIT_R: return ">>";
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LTE: return "<=";
        case OP_GTE: return ">=";
        case OP_EQU: return "==";
        case OP_NEQ: return "!=";
        case OP_BIT_AND: return "& ";
        case OP_BIT_XOR: return "^";
        case OP_BIT_OR: return "|";
        case OP_AND: return "&&";
        case OP_OR: return "||";
        default: return "UNKNOWN_OP";
    }
}
char* valTypeToString(ValueType val) {
    switch (val) {
        case VALUE_OP: return "OP";
        case VALUE_NUM: return "NUM";
        case VALUE_VAR: return "VAR";
        case VALUE_FUNC_RET: return "FUNC_RET";
        case VALUE_MATH_OP: return "MATH_OP";
        default: return "UNKNOWN";
    }
}
void printValueNode(ValueNode* value) {
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
            printf("VAR: %s ", ((VarEntry*)value->value)->name);
            break;
        case VALUE_FUNC_RET:
            printf("FUNC: %s", ((FuncCallNode*)value->value)->funcName);
            break;
        case VALUE_MATH_OP:
            if (((MathOpNode*)value->value)->left == NULL)
                printf("MathOp: Left: NULL | Right: %s | OP: %s", valTypeToString(((MathOpNode*)value->value)->right->valueType), opToString(((MathOpNode*)value->value)->opType));
            else
                printf("MathOp: Left: %s | Right: %s | OP: %s", valTypeToString(((MathOpNode*)value->value)->left->valueType), 
                    valTypeToString(((MathOpNode*)value->value)->right->valueType), opToString(((MathOpNode*)value->value)->opType));
            break;
        default:
            printf("Unknown value type in value print.\n");
    }
    return;
}
void printPostfix(queueOrStackNode* head) {
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
void printASTs(ASTNode* head, int tabs) {
    if (head == NULL) {
        printf("NULL AST\n");
        return;
    }
    ASTNode* current = head;
    while (current != NULL) {
        for (int i = 0; i < tabs; i++) printf("\t");
        switch (current->nodeType) {
            case NODE_VAR_DECL:
                printf("VarDecl: %s", ((VarDeclNode*)current->subNode)->varInfo->name);
                printf("\n");
                break;
            case NODE_VAR_ASSIGN:
                printf("VarAssign: %s -> ", ((VarAssignNode*)current->subNode)->varInfo->name);
                printValueNode(((VarAssignNode*)current->subNode)->newValue);
                printf("\n");
                break;
            case NODE_FUNC_DECL:
                printf("FuncDecl: %s(%d args)\n", ((FuncDeclNode*)current->subNode)->funcInfo->name, ((FuncDeclNode*)current->subNode)->funcInfo->paramCount);
                printASTs(((FuncDeclNode*)current->subNode)->body, tabs + 1);
                break;
            case NODE_WHILE:
                printf("While: ( ");
                printValueNode(((WhileNode*)current->subNode)->condition);
                printf(" )\n");
                printASTs(((WhileNode*)current->subNode)->body, tabs + 1);
                break;
            case NODE_IF:
                printf("If: ( ");
                printValueNode(((IfNode*)current->subNode)->condition);
                printf(" )\n");
                printASTs(((IfNode*)current->subNode)->body, tabs + 1);
                if (((IfNode*)current->subNode)->elseBody != NULL) {
                    for (int i = 0; i < tabs; i++) printf("\t");
                    printf("Else:\n");
                    printASTs(((IfNode*)current->subNode)->elseBody, tabs + 1);
                }
                break;
            case NODE_RETURN:
                printf("Return: ");
                printValueNode(((ReturnNode*)current->subNode)->returnValue);
                printf("\n");
                break;
            default:
                printf("Unknown node type in AST print.\n");
        }
        current = current->next;
    }
    return;
}
//FREE FUNCTIONS
void freeMathOpNode(MathOpNode* head) {
    if (head == NULL) return;
    if (head->left != NULL) freeValueNode(head->left);
    if (head->right != NULL) freeValueNode(head->right);
    free(head);
}
void freeValueNode(ValueNode* head) {
    if (head == NULL) return;
    switch (head->valueType) {
        case VALUE_OP:
        case VALUE_NUM:
            free(head->value);
            break;
        case VALUE_VAR:
            break; //these get freed from their declarations in the AST
        case VALUE_FUNC_RET:
            free(((FuncCallNode*)head->value)->funcName);
            for (int i = 0; i < ((FuncCallNode*)head->value)->argCount; i++) {
                freeValueNode(((FuncCallNode*)head->value)->args[i]);
            }
            free(((FuncCallNode*)head->value)->args);
            free(head->value);
            break;
        case VALUE_MATH_OP:
            freeMathOpNode(head->value);
            break;
        default:
            printf("Unknown value type in freeValueNode.\n");
    }
    free(head);
}
void freePostfix(queueOrStackNode* head) { //free the postfix expression (but not the valueNodes created)
    if (head == NULL) return;
    queueOrStackNode* current = head;
    while (current != NULL) {
        queueOrStackNode* next = current->next;
        free(current);
        current = next;
    }
}
void freeASTNodes(ASTNode* head) { //frees everything else in the AST
    if (head == NULL) return;
    ASTNode* current = head;
    while (current != NULL) {
        ASTNode* next = current->next;
        switch (current->nodeType) {
            case NODE_VAR_DECL:
                free(((VarDeclNode*)current->subNode)->varInfo->name);
                free(((VarDeclNode*)current->subNode)->varInfo);
                break;
            case NODE_VAR_ASSIGN: //varInfo freed in declaration of var
                freeValueNode(((VarAssignNode*)current->subNode)->newValue);
                break;
            case NODE_FUNC_DECL:
                free(((FuncDeclNode*)current->subNode)->funcInfo->name);
                for (int i = 0; i < ((FuncDeclNode*)current->subNode)->funcInfo->paramCount; i++) {
                    free(((FuncDeclNode*)current->subNode)->paramList[i]->name);
                    free(((FuncDeclNode*)current->subNode)->paramList[i]);
                }
                free(((FuncDeclNode*)current->subNode)->funcInfo);
                free(((FuncDeclNode*)current->subNode)->paramList);
                freeASTNodes(((FuncDeclNode*)current->subNode)->body);
                break;
            case NODE_WHILE:
                freeValueNode(((WhileNode*)current->subNode)->condition);
                freeASTNodes(((WhileNode*)current->subNode)->body);
                break;
            case NODE_IF:
                freeValueNode(((IfNode*)current->subNode)->condition);
                freeASTNodes(((IfNode*)current->subNode)->body);
                freeASTNodes(((IfNode*)current->subNode)->elseBody);
                break;
            case NODE_RETURN:
                freeValueNode(((ReturnNode*)current->subNode)->returnValue);
                break;
            default:
                printf("Unknown node type in AST free.\n");
        }
        free(current->subNode);
        free(current);
        current = next;
    }
}
//HELPER FUNCTIONS
Token* skipToSeperator(Token* head, bool seperatorType) { //seperator type is true to skip to semicolon, false to skip to comma
    if (head == NULL) {
        printf("\033[1;31mNull parameter to skipToSeperator.\033[0m\n");
        return NULL;
    }
    for (Token* current = head; current != NULL; current = current->nextToken) {
        if ((current->value[0] == ';' && seperatorType) || (current->value[0] == ',' && !seperatorType)) return current;
    }
    return NULL;
}
Token* findMatchingParen(Token* head, bool parenType) { //takes in first paren and returns matching end paren - parenType is true for ( and false for {
    if (head == NULL || head->value == NULL) {
        printf("\033[1;31mNull parameter to findMatchingParen.\033[0m\n");
        return NULL;
    }
    if ((head->value[0] != '(' && parenType) || (head->value[0] != '{' && !parenType)) {
        printf("\033[1;31mBad start token %s to findMatchingParen(%d).\033[0m\n", head->value, parenType);
        return NULL;
    }
    int parens = 1;
    for (Token* current = head->nextToken; current != NULL && parens > 0; current = current->nextToken) {
        if ((current->value[0] == '(' && parenType) || (current->value[0] == '{' && !parenType)) parens++;
        else if ((current->value[0] == ')' && parenType) || (current->value[0] == '}' && !parenType)) parens--;
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
OperatorType getOperatorType(Token* token, VarTable* varTable, FuncTable* funcTable) { //return values are in enum declaration
    if (token == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to getOperatorType.\033[0m\n");
        return NULL_OP;
    }
    switch (token->tokenType) {
        case 0: //identifier (var/func)
            if (token->nextToken->value[0] == '(') { //func
                if (funcLookup(funcTable, token->value, numParams(token->nextToken)) != NULL) //is func defined?
                    return VAL_FUNC;
                printf("\033[1;31mUndefined function in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                return NULL_OP;
            }
            else { //var
                if (varLookup(varTable, token->value) != NULL) //is var defined?
                    return VAL_VAR;
                printf("\033[1;31mUndefined variable in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                return NULL_OP;
            }
        case 1: //number
            return VAL_NUM;
        case 2: //operator
            switch (token->value[0]) {
                case '=': 
                    if (token->value[1] == '=') 
                        return OP_EQU; // "=="
                    printf("\033[1;31mInvalid operator = in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                    return NULL_OP; // "=" (error)
                case '~': return OP_BIT_NOT;
                case '!': 
                    if (token->value[1] == '=')
                        return OP_NEQ; // "!="
                    return OP_NOT; // "!"
                case '&': 
                    if (token->value[1] == '&') 
                        return OP_AND; // "&&"
                    if (token->value[1] == ' ') 
                        return OP_BIT_AND; // "& " (with a space)
                    return OP_REF; // "&"
                case '*':
                    if (token->value[1] == ' ') 
                        return OP_MUL; // "* " (with a space)
                    return OP_DEREF; // "*"
                case '/': return OP_DIV;
                case '%': return OP_MOD;
                case '+': return OP_ADD;
                case '-': return OP_SUB;
                case '<':
                    if (token->value[1] == '<') 
                        return OP_BIT_L; // "<<"
                    if (token->value[1] == '=') 
                        return OP_LTE; // "<="
                    return OP_LT; // "<"
                case '>':
                    if (token->value[1] == '>') 
                        return OP_BIT_R; // ">>"
                    if (token->value[1] == '=') 
                        return OP_GTE; // ">="
                    return OP_GT; // ">"
                case '^': return OP_BIT_XOR;
                case '|': 
                    if (token->value[1] == '|') 
                        return OP_OR; // "||"
                    return OP_BIT_OR; // "|"
                default: 
                    printf("\033[1;31mUnknown operator in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
                    return NULL_OP; //unknown operator (error)
            }
        case 3: 
            printf("\033[1;31mKeyword in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return NULL_OP; //keyword (error)
        case 4:
            if (token->value[0] == '(') 
                return OP_OPEN_PAREN;
            if (token->value[0] == ')') 
                return OP_CLOSE_PAREN;
            printf("\033[1;31mBad seperator in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return NULL_OP; //other seperator (error)
        default:
            printf("\033[1;31mUnknown token type in math operation. Line %d. Value %s.\033[0m\n", token->lineNum, token->value);
            return NULL_OP; //unknown token (error)
    }
}
int getArgType(Token* head, Token* tail) { //head is first token of arg and tail is last token of arg (neither should be a parenthesis). -1 error, 0 simple arg, 1 func call, 2 math op
    if (head == NULL || tail == NULL) {
        printf("\033[1;31mNull parameter to getArgType.\033[0m\n");
        return -1;
    }
    if (head == tail) { //1 token argument (simple arg)
        return 0;
    }
    if (head->tokenType == TOKEN_IDENTIFIER && head->nextToken->value[0] == '(') {
        //go to end of call to determine if function call or math op that starts with func call
        Token* endOfCall = findMatchingParen(head->nextToken, true);
        if (endOfCall == NULL) return -1; //error
        if (endOfCall == tail) return 1; //function call
    }
    return 2; //math op
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
    queueOrStackNode* newFront = front->next;
    free(front);
    return newFront;
}
queueOrStackNode* push(queueOrStackNode* front, ValueNode* value) { //return new front
    queueOrStackNode* newNode = (queueOrStackNode*)malloc(sizeof(queueOrStackNode));
    if (newNode == NULL) {
        printf("\033[1;31mMalloc error in push.\033[0m\n");
        return NULL;
    }
    newNode->value = value;
    newNode->next = front;
    return newNode;
}
//PARSING MATH OPERATIONS
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
        switch (opType) {
            case NULL_OP: //error
                return NULL; 
            case VAL_NUM: { //number
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
                break;
            }
            case VAL_VAR: { //var
                ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
                if (newValNode == NULL) {
                    printf("\033[1;31mMalloc error in buildPostfix.\033[0m\n");
                    return NULL;
                }
                newValNode->valueType = VALUE_VAR;
                newValNode->value = varLookup(varTable, current->value);
                if (newValNode->value == NULL) {
                    printf("\033[1;31mUndefined variable in math operation. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                    return NULL;
                }
                outQBack = enqueue(outQBack, newValNode);
                if (outQBack == NULL) {
                    printf("\033[1;31mEnqueue Failed.\033[0m\n");
                    return NULL;
                }
                if (outQ == NULL) outQ = outQBack;
                break;
            }
            case VAL_FUNC: { //func
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
                    newValNode->value = parseFuncCall(current->nextToken, varTable, funcTable, params);
                    if (newValNode->value == NULL) {
                        printf("\033[1;31mparseFuncCall failed on function call in line %d.\033[0m\n", current->lineNum);
                        return NULL;
                    }
                }
                outQBack = enqueue(outQBack, newValNode);
                if (outQBack == NULL) {
                    printf("\033[1;31mEnqueue Failed.\033[0m\n");
                    return NULL;
                }
                if (outQ == NULL) outQ = outQBack;
                current = findMatchingParen(current->nextToken, true);
                if (current == NULL) {
                    printf("\033[1;31mError skipping function parameters in buildPostfix.\033[0m\n");
                    return NULL;
                }
                break;
            }
            case OP_OPEN_PAREN: {
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
                break;
            }
            case OP_CLOSE_PAREN: {
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
                break;
            }
            default: { //operator
                if (opType/4 == 1) { //unary op
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
                else { //binary op
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
                break;
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
                nodeStack = push(nodeStack, current->value);
                break;
            case VALUE_MATH_OP:
                printf("\033[1;31mMathOpNode in postfixToTree.\033[0m\n");
                return NULL;
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
    if (currentTokenType == NULL_OP) {
        printf("\033[1;31mError token encountered in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", endTok->lineNum, endTok->value);
        return false;
    }
    if (currentTokenType > NULL_OP && currentTokenType != OP_CLOSE_PAREN) { //can't end with operator or open paren
        printf("\033[1;31mExpression ends with operator or open paren (isValidMathOp). Line %d. Value %s.\033[0m\n", endTok->lineNum, endTok->value);
        return false;
    }
    currentTokenType = getOperatorType(head, varTable, funcTable);
    if (currentTokenType == NULL_OP) {
        printf("\033[1;31mError token encountered math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return false;
    }
    if (currentTokenType == OP_CLOSE_PAREN || currentTokenType >= OP_MUL) { //can't start with binary op or close paren
        printf("\033[1;31mExpression starts with a binary op or close paren (isValidMathOp). Line %d. Value %s.\033[0m\n", head->lineNum, head->value);
        return false;
    }
    if (currentTokenType == VAL_FUNC) {//skip to end of func call
        head = findMatchingParen(head->nextToken, true);
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
        if (currentTokenType == NULL_OP) { //invalid token
            printf("\033[1;31mError token encountered in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
            return false;
        }
        if (currentTokenType < NULL_OP) { //operand
            if (previousTokenType < NULL_OP) {
                printf("\033[1;31mSequential operands in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
            if (previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mOperand following close paren in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType == OP_OPEN_PAREN) { //open parenthesis
            if (previousTokenType < NULL_OP) {
                printf("\033[1;31mOpen paren following operand in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
            if (previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mOpen paren following close paren in math operation (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType == OP_CLOSE_PAREN) { //close parenthesis
            if (previousTokenType > NULL_OP) {
                printf("\033[1;31mClose paren following operator (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_BIT_NOT && currentTokenType <= OP_DEREF) { //unary operator
            if (previousTokenType < NULL_OP || previousTokenType == OP_CLOSE_PAREN) {
                printf("\033[1;31mUnary operator following operand or close parenthesis (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        else if (currentTokenType >= OP_MUL) { //binary operator
            if (previousTokenType > NULL_OP && previousTokenType != OP_CLOSE_PAREN) {
                printf("\033[1;31mBinary operator following another operator or open parenthesis (isValidMathOp). Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return false;
            }
        }
        if (currentTokenType == VAL_FUNC) { //skip to end of func call
            current = findMatchingParen(current->nextToken, true);
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
    if (!isValidMathOp(head, endTok, varTable, funcTable)) { //check for valid token sequences
        printf("\033[1;31mError when validating math operation. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }
    queueOrStackNode* postfix = buildPostfix(head, endTok, varTable, funcTable); //build postfix (deep copied values from tokens)
    if (postfix == NULL) {
        printf("\033[1;31mError building postfix expression. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }

    MathOpNode* mathOpTree = postfixToTree(postfix); //build binary tree from postfix (deep copy values from postfix then free postfix)
    if (mathOpTree == NULL) {
        printf("\033[1;31mError building AST from postfix expression. Line %d.\033[0m\n", head->lineNum);
        return NULL;
    }
    return mathOpTree;
}
//PARSE FUNCTION ARGUMENTS
FuncCallNode* parseFuncCall(Token* head, VarTable* varTable, FuncTable* funcTable, int numArgs) { //takes in first paren of call and returns call formatted as func call node
    if (head == NULL || varTable == NULL || funcTable == NULL || numArgs < 0) {
        printf("\033[1;31mBad parameter to parseFuncCall.\033[0m\n");
        return NULL;
    }
    if (head->value[0] != '(') {
        printf("\033[1;31mBad start token to parseFuncCall.\033[0m\n");
        return NULL;
    }
    //allocate memory
    FuncCallNode* funcCall = (FuncCallNode*)malloc(sizeof(FuncCallNode));
    if (funcCall == NULL) {
        printf("\033[1;31mMalloc error in parseFuncCall.\033[0m\n");
        return NULL;
    }
    funcCall->funcName = (char*)malloc(strlen(head->prevToken->value) + 1);
    if (funcCall->funcName == NULL) {
        printf("\033[1;31mMalloc error in parseFuncCall.\033[0m\n");
        return NULL;
    }
    strcpy(funcCall->funcName, head->prevToken->value);
    funcCall->argCount = numArgs;
    if (numArgs == 0) {
        funcCall->args = NULL;
        return funcCall;
    }
    funcCall->args = (ValueNode**)malloc(numArgs * sizeof(ValueNode*));
    if (funcCall->args == NULL) {
        printf("\033[1;31mMalloc error in parseFuncCall.\033[0m\n");
        return NULL;
    }
    //seperate arguments
    Token* startToken = head->nextToken;
    for (int i = 0; i < numArgs; i++) { //each iteration starts on first token of the argument
        if (startToken == NULL) {
            printf("\033[1;31mUnexpected end of file in parseFuncCall.\033[0m\n");
            return NULL;
        }
        int parens = 1;
        Token* endToken = startToken;
        while (true) { //move end to end of argument seperator (close paren or comma)
            if (endToken == NULL || endToken->value[0] == ';') {
                printf("\033[1;31mError within argument in parseFuncCall.\033[0m\n");
                return NULL;
            }
            if (endToken->value[0] == '(') parens++;
            if (endToken->value[0] == ')')
                if (--parens == 0) break;
            if (endToken->value[0] == ',' && parens == 1) break;
            endToken = endToken->nextToken;
        }
        //parse the argument
        funcCall->args[i] = parseValueNode(startToken, endToken->prevToken, varTable, funcTable);
        if (funcCall->args[i] == NULL) {
            printf("\033[1;31mError parsing argument in parseFuncCall.\033[0m\n");
            return NULL;
        }
        startToken = endToken->nextToken;
    }
    return funcCall;
}
//PARSE SUB NODES
ValueNode* parseValueNode(Token* head, Token* endTok, VarTable* varTable, FuncTable* funcTable) { //takes first value in expression and last value in expression (inclusive) and returns AST
    if (head == NULL || endTok == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseValueNode.\033[0m\n");
        return NULL;
    }
    ValueNode* newValNode = (ValueNode*)malloc(sizeof(ValueNode));
    if (newValNode == NULL) {
        printf("\033[1;31mMalloc error in parseValueNode.\033[0m\n");
        return NULL;
    }
    switch (getArgType(head, endTok)) {
        case 0:  //simple value - handle here
            if (head->tokenType == TOKEN_NUMBER) { //number literal
                newValNode->valueType = VALUE_NUM;
                newValNode->value = malloc(sizeof(long long));
                if (newValNode->value == NULL) {
                    printf("\033[1;31mMalloc error in parseValueNode.\033[0m\n");
                    return NULL;
                }
                *(long long*)newValNode->value = atoll(head->value);
            }
            else if (head->tokenType == TOKEN_IDENTIFIER) { //variable
                newValNode->valueType = VALUE_VAR;
                newValNode->value = varLookup(varTable, head->value);
                if (newValNode->value == NULL) {
                    printf("\033[1;31mUndefined variable in parseValueNode.\033[0m\n");
                    return NULL;
                }
            }
            else {
                printf("\033[1;31mBad token type in parseValueNode.\033[0m\n");
                return NULL;
            }
            break;
        case 1: //function call - use helper function
            newValNode->valueType = VALUE_FUNC_RET;
            int params = numParams(head->nextToken);
            if (params == -1) {
                printf("\033[1;31mInvalid function call in parseValueNode.\033[0m\n");
                return NULL;
            }
            if (!funcLookup(funcTable, head->value, params)) {
                printf("\033[1;31mUndefined function %s in parseValueNode.\033[0m\n", head->value);
                return NULL;
            }
            newValNode->value = parseFuncCall(head->nextToken, varTable, funcTable, params);
            if (newValNode->value == NULL) {
                printf("\033[1;31mError parsing function call in parseValueNode.\033[0m\n");
                return NULL;
            }
            break;
        case 2: //math operation - use helper function
            newValNode->valueType = VALUE_MATH_OP;
            newValNode->value = parseMathOp(head, endTok, varTable, funcTable);
            if (newValNode->value == NULL) {
                printf("\033[1;31mError parsing math operation in parseValueNode.\033[0m\n");
                return NULL;
            }
            break;
        default:
            printf("\033[1;31mBad argument type from getArgType in parseValueNode.\033[0m\n");
            return NULL;
    }
    return newValNode;
}
VarAssignNode* parseVarAssign(Token* head, VarTable* varTable, FuncTable* funcTable) { //statement starts with an identifier
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseVarAssign.\033[0m\n");
        return NULL;
    }
    if (varLookup(varTable, head->value) == NULL) {
        printf("\033[1;31mUndefined variable in parseVarAssign.\033[0m\n");
        return NULL;
    }
    if (head->nextToken == NULL || head->nextToken->value[0] != '=') {
        printf("\033[1;31mBad assignment operator in parseVarAssign.\033[0m\n");
        return NULL;
    }
    VarAssignNode* newVarAssignNode = (VarAssignNode*)malloc(sizeof(VarAssignNode));
    if (newVarAssignNode == NULL) {
        printf("\033[1;31mMalloc error in parseVarAssign.\033[0m\n");
        return NULL;
    }
    newVarAssignNode->varInfo = varLookup(varTable, head->value);
    if (newVarAssignNode->varInfo == NULL) {
        printf("\033[1;31mUndefined variable in parseVarAssign.\033[0m\n");
        return NULL;
    }
    newVarAssignNode->newValue = parseValueNode(head->nextToken->nextToken, skipToSeperator(head, true)->prevToken, varTable, funcTable);
    if (newVarAssignNode->newValue == NULL) {
        printf("\033[1;31mError parsing value in parseVarAssign.\033[0m\n");
        return NULL;
    }
    return newVarAssignNode;
}
VarDeclNode* parseVarDecl(Token* head, VarTable* varTable) { //statement starts with the var keyword
    if (head == NULL || varTable == NULL) {
        printf("\033[1;31mNull parameter to parseVarDecl.\033[0m\n");
        return NULL;
    }
    if (head->nextToken == NULL || head->nextToken->tokenType != TOKEN_IDENTIFIER) {
        printf("\033[1;31mBad variable name in parseVarDecl.\033[0m\n");
        return NULL;
    }
    if (head->nextToken->nextToken == NULL || head->nextToken->nextToken->value[0] != ';') {
        printf("\033[1;31mBad assignment in parseVarDecl.\033[0m\n");
        return NULL;
    }
    VarDeclNode* newVarDeclNode = (VarDeclNode*)malloc(sizeof(VarDeclNode));
    if (newVarDeclNode == NULL) {
        printf("\033[1;31mMalloc error in parseVarDecl.\033[0m\n");
        return NULL;
    }
    newVarDeclNode->varInfo = pushVarEntry(varTable, head->nextToken->value);
    if (newVarDeclNode->varInfo == NULL) {
        printf("\033[1;31mError adding variable to table in parseVarDecl.\033[0m\n");
        return NULL;
    }
    return newVarDeclNode;
}
IfNode* parseIf(Token* head, VarTable* varTable, FuncTable* funcTable) { //statement starts with the if keyword
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseIf.\033[0m\n");
        return NULL;
    }
    if (head->nextToken->value[0] != '(') {
        printf("\033[1;31mBad start to condition.\033[0m\n");
        return NULL;
    }
    IfNode* newIfNode = (IfNode*)malloc(sizeof(IfNode));
    if (newIfNode == NULL) {
        printf("\033[1;31mMalloc error in parseIf.\033[0m\n");
        return NULL;
    }
    Token* conditionEnd = findMatchingParen(head->nextToken, true); //end ")" of if condition
    if (conditionEnd == NULL) {
        printf("\033[1;31mError finding end of condition in parseIf.\033[0m\n");
        return NULL;
    }
    newIfNode->condition = parseValueNode(head->nextToken->nextToken, conditionEnd->prevToken, varTable, funcTable);
    if (newIfNode->condition == NULL) {
        printf("\033[1;31mError parsing condition in parseIf.\033[0m\n");
        return NULL;
    }
    newIfNode->body = parseTokens(conditionEnd->nextToken->nextToken, false, varTable, funcTable);
    if (newIfNode->body == NULL) {
        printf("\033[1;31mError parsing body in parseIf.\033[0m\n");
        return NULL;
    }
    conditionEnd = findMatchingParen(conditionEnd->nextToken, false); //end "}" of if body
    if (conditionEnd == NULL) {
        printf("\033[1;31mError finding end of body in parseIf.\033[0m\n");
        return NULL;
    }
    if (conditionEnd->nextToken != NULL && conditionEnd->nextToken->tokenType == TOKEN_KEYWORD && conditionEnd->nextToken->value[0] == 'e') { //has an else statement
        newIfNode->elseBody = parseTokens(conditionEnd->nextToken->nextToken->nextToken, false, varTable, funcTable);
        if (newIfNode->elseBody == NULL) {
            printf("\033[1;31mError parsing else body in parseIf.\033[0m\n");
            return NULL;
        }
    }
    else newIfNode->elseBody = NULL;

    return newIfNode;
}
WhileNode* parseWhile(Token* head, VarTable* varTable, FuncTable* funcTable) { //statement starts with the while keyword
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseWhile.\033[0m\n");
        return NULL;
    }
    if (head->nextToken->value[0] != '(') {
        printf("\033[1;31mBad start to condition.\033[0m\n");
        return NULL;
    }
    WhileNode* newWhileNode = (WhileNode*)malloc(sizeof(WhileNode));
    if (newWhileNode == NULL) {
        printf("\033[1;31mMalloc error in parseWhile.\033[0m\n");
        return NULL;
    }
    Token* conditionEnd = findMatchingParen(head->nextToken, true); //end ")" of while condition
    if (conditionEnd == NULL) {
        printf("\033[1;31mError finding end of condition in parseWhile.\033[0m\n");
        return NULL;
    }
    newWhileNode->condition = parseValueNode(head->nextToken->nextToken, conditionEnd->prevToken, varTable, funcTable);
    if (newWhileNode->condition == NULL) {
        printf("\033[1;31mError parsing condition in parseWhile.\033[0m\n");
        return NULL;
    }
    newWhileNode->body = parseTokens(conditionEnd->nextToken->nextToken, false, varTable, funcTable);
    if (newWhileNode->body == NULL) {
        printf("\033[1;31mError parsing body in parseWhile.\033[0m\n");
        return NULL;
    }
    return newWhileNode;
}
FuncDeclNode* parseFuncDef(Token* head, FuncTable* funcTable) { //statement starts with the func keyword
    if (head == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseFuncDef.\033[0m\n");
        return NULL;
    }
    if (head->nextToken == NULL || head->nextToken->tokenType != TOKEN_IDENTIFIER) {
        printf("\033[1;31mBad function name in parseFuncDef.\033[0m\n");
        return NULL;
    }
    if (head->nextToken->nextToken == NULL || head->nextToken->nextToken->value[0] != '(') {
        printf("\033[1;31mBad start to function parameters in parseFuncDef.\033[0m\n");
        return NULL;
    }
    FuncDeclNode* newFuncDeclNode = (FuncDeclNode*)malloc(sizeof(FuncDeclNode));
    if (newFuncDeclNode == NULL) {
        printf("\033[1;31mMalloc error in parseFuncDef.\033[0m\n");
        return NULL;
    }
    int params = numParams(head->nextToken->nextToken);
    if (params == -1) {
        printf("\033[1;31mError counting function parameters in parseFuncDef.\033[0m\n");
        return NULL;
    }
    newFuncDeclNode->funcInfo = pushFuncEntry(funcTable, head->nextToken->value, params);
    if (newFuncDeclNode->funcInfo == NULL) {
        printf("\033[1;31mError adding function to table in parseFuncDef.\033[0m\n");
        return NULL;
    }
    if (params == 0) { //no parameters
        newFuncDeclNode->paramList = NULL;
        newFuncDeclNode->body = parseTokens(head->nextToken->nextToken->nextToken->nextToken->nextToken, false, createVarTable(), funcTable);
        if (newFuncDeclNode->body == NULL) {
            printf("\033[1;31mError parsing body in parseFuncDef.\033[0m\n");
            return NULL;
        }
    }
    else { //parameters - make a new table and special parameter scope
        newFuncDeclNode->paramList = (VarEntry**)malloc(params * sizeof(VarEntry*));
        if (newFuncDeclNode->paramList == NULL) {
            printf("\033[1;31mMalloc error in parseFuncDef.\033[0m\n");
            return NULL;
        }
        VarTable* newVarTable = createVarTable();
        if (newVarTable == NULL) {
            printf("\033[1;31mError creating new table in parseFuncDef.\033[0m\n");
            return NULL;
        }
        if (pushVarScope(newVarTable, true) == false) {
            printf("\033[1;31mError pushing scope in parseFuncDef.\033[0m\n");
            return NULL;
        }
        Token* argStart = head->nextToken->nextToken->nextToken;
        for (int i = 0; i < params; i++) { //get the argument names - ends on { of function body
            if (argStart == NULL || argStart->tokenType != TOKEN_IDENTIFIER) {
                printf("\033[1;31mBad argument name in parseFuncDef.\033[0m\n");
                return NULL;
            }
            newFuncDeclNode->paramList[i] = pushVarEntry(newVarTable, argStart->value);
            if (newFuncDeclNode->paramList[i] == NULL) {
                printf("\033[1;31mError adding argument to table in parseFuncDef.\033[0m\n");
                return NULL;
            }
            argStart = argStart->nextToken;
            if (argStart == NULL || (argStart->value[0] != ',' && argStart->value[0] != ')')) {
                printf("\033[1;31mBad argument seperator in parseFuncDef.\033[0m\n");
                return NULL;
            }
            argStart = argStart->nextToken;
        }
        newFuncDeclNode->body = parseTokens(argStart->nextToken, false, newVarTable, funcTable);
        if (newFuncDeclNode->body == NULL) {
            printf("\033[1;31mError parsing body in parseFuncDef.\033[0m\n");
            return NULL;
        }
    }

    return newFuncDeclNode;
}
ReturnNode* parseReturn(Token* head, VarTable* varTable, FuncTable* funcTable) { //statement starts with the return keyword
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseReturn.\033[0m\n");
        return NULL;
    }
    if (head->nextToken == NULL || head->nextToken->value[0] == ';') {
        printf("\033[1;31mEmpty return statement in parseReturn.\033[0m\n");
        return NULL;
    }
    ReturnNode* newReturnNode = (ReturnNode*)malloc(sizeof(ReturnNode));
    if (newReturnNode == NULL) {
        printf("\033[1;31mMalloc error in parseReturn.\033[0m\n");
        return NULL;
    }
    newReturnNode->returnValue = parseValueNode(head->nextToken, skipToSeperator(head, true)->prevToken, varTable, funcTable);
    if (newReturnNode->returnValue == NULL) {
        printf("\033[1;31mError parsing return value in parseReturn.\033[0m\n");
        return NULL;
    }
    return newReturnNode;
}
//MAIN PARSING FUNCTION
ASTNode* parseTokens(Token* head, bool inGlobalScope, VarTable* varTable, FuncTable* funcTable) { //head is token after {
    if (head == NULL || varTable == NULL || funcTable == NULL) {
        printf("\033[1;31mNull parameter to parseTokens.\033[0m\n");
        return NULL;
    }

    Token* endTok = NULL;
    if (!inGlobalScope) { //look for ending bracket if not in global scope
        endTok = findMatchingParen(head->prevToken, false);
        if (endTok == NULL) {
            printf("\033[1;31mError finding end token in parseTokens.\033[0m\n");
            return NULL;
        }
    }

    if (!pushVarScope(varTable, false)) { //push a new scope for local variables
        printf("\033[1;31mError pushing scope in parseTokens.\033[0m\n");
        return NULL;
    }

    ASTNode* prevNode = NULL;
    ASTNode* ASTHead = NULL;
    ASTNode* newASTNode = NULL;
    Token* current;
    for (current = head; current != endTok && current != NULL; current = current->nextToken) {
        newASTNode = (ASTNode*)malloc(sizeof(ASTNode));
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
                current = skipToSeperator(current, true);
                if (current == NULL) {
                    printf("\033[1;31mError skipping to semicolon in parseTokens.\033[0m\n");
                    return NULL;
                }
                break;
            case TOKEN_KEYWORD: //start of special statement
                switch(current->value[0]) {
                    case 'v': //variable declaration
                        newASTNode->nodeType = NODE_VAR_DECL;
                        newASTNode->subNode = parseVarDecl(current, varTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing variable declaration. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        current = skipToSeperator(current, true);
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to semicolon in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        break;
                    case 'i': //if statement
                        newASTNode->nodeType = NODE_IF;
                        newASTNode->subNode = parseIf(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing if statement. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken, true); //skip the condition
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of if statement in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken, false); //skip the body
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of if statement in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        if (current->nextToken != NULL && current->nextToken->tokenType == TOKEN_KEYWORD && current->nextToken->value[0] == 'e') { //has an else statement
                            current = findMatchingParen(current->nextToken->nextToken, false); //skip the else body
                            if (current == NULL) {
                                printf("\033[1;31mError skipping to end of else statement in parseTokens.\033[0m\n");
                                return NULL;
                            }
                        }
                        break;
                    case 'w': //while loop
                        newASTNode->nodeType = NODE_WHILE;
                        newASTNode->subNode = parseWhile(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing while loop. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken, true); //skip the condition
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of while loop in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken, false); //skip the body
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of while loop in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        break;
                    case 'f': //function definition
                        newASTNode->nodeType = NODE_FUNC_DECL;
                        newASTNode->subNode = parseFuncDef(current, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing function definition. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken->nextToken, true); //skip the parameter declarations
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of function definition in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        current = findMatchingParen(current->nextToken, false); //skip the body
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to end of function body in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        break;
                    case 'r': //return statement
                        newASTNode->nodeType = NODE_RETURN;
                        newASTNode->subNode = parseReturn(current, varTable, funcTable);
                        if (newASTNode->subNode == NULL) {
                            printf("\033[1;31mError parsing return statement. Line %d.\033[0m\n", current->lineNum);
                            return NULL;
                        }
                        current = skipToSeperator(current, true);
                        if (current == NULL) {
                            printf("\033[1;31mError skipping to semicolon in parseTokens.\033[0m\n");
                            return NULL;
                        }
                        break;
                    default:
                        printf("\033[1;31mIllegal keyword in parseTokens. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                }
                break;
            default:
                printf("\033[1;31mIllegal start of statement. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
                return NULL;
        }
        //update linked list
        if (prevNode == NULL) ASTHead = newASTNode;
        else prevNode->next = newASTNode;
        prevNode = newASTNode;
    }
    if (!inGlobalScope && current == NULL) {
        printf("\033[1;31mUnexpected end of file in parseTokens.\033[0m\n");
        return NULL;
    }
    
    popVarScope(varTable); //pop scope
    return ASTHead;
}