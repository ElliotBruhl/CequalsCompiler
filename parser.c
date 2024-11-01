#include "parser.h"
#include "symbolTable.h"
#include <stdlib.h>

void freeASTNodes(ASTNode* head) {
    //...
}
void printASTs(ASTNode* head) {
    //...
}
/*
typedef enum { //divide by 4 to get precedence level
    // Special
    OP_ASSIGN = 0,      // = (only for assignment and declaration)
    // Precedence 1 - parentheses (a separator) -------- FIRST PRECEDENCE
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
*/
OperatorType getOperatorType(Token* token, SymbolTable* table) { //-1 for number, 0 for error, other for valid operator (do op/4 for precedence)
    switch (token->tokenType) {
        case 0: //var or func
            if (lookup(table, token->value, -1) != NULL) return -2; //number
            else if (/*look for func with correct args*/0) return -3; //func
            else return 0; //error
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

Token* buildPostfix(Token* head, Token* endTok, SymbolTable* table) { //takes in math expression (terminated correctly with end token)
    if (head == NULL || endTok == NULL) return NULL;
    Token* outQueueFront = NULL;
    Token* outQueueBack = NULL;
    Token* opStackBack = NULL; //pointer to top of stack

    for (Token* current = head; current != endTok; current = current->nextToken) {
        OperatorType opType = getOperatorType(current, table);
        if (opType == 0) {
            printf("\033[1;31mInvalid token in math operation. Line %d. Value %s.\033[0m\n", current->lineNum, current->value);
            return NULL;
        }
        if (opType == -1) {

        }
    }

    return outQueueFront;
}
//BUILD POSTFIX EXPRESSION
/*
1. Define two data structures:
    - `outputQueue`: List to store the final postfix expression.
    - `operatorStack`: Stack to temporarily hold operators and parentheses.

2. Define a `precedence(op)` function to return precedence level for each operator. Associativity is l->r unless op 4,5,6, or 7

4. For each token in the input:
    - If the token is an operand (number, variable):
        - Add it to `outputQueue`.

    - Else if the token is a unary operator (e.g., `~`, `!`, `*` for dereference):
        - While `operatorStack` is not empty and:
            - The token at the top of `operatorStack` has higher precedence, or
            - The token at the top has equal precedence and is left associative,
          - Pop operators from `operatorStack` to `outputQueue`.
        - Push the current unary operator onto `operatorStack`.

    - Else if the token is a binary operator (e.g., `+`, `-`, `*`, `/`):
        - While `operatorStack` is not empty and:
            - The token at the top of `operatorStack` has higher precedence, or
            - The token at the top has equal precedence and is left associative,
          - Pop operators from `operatorStack` to `outputQueue`.
        - Push the current binary operator onto `operatorStack`.

    - Else if the token is a left parenthesis `(`:
        - Push it onto `operatorStack`.

    - Else if the token is a right parenthesis `)`:
        - While the token at the top of `operatorStack` is not a left parenthesis:
            - Pop operators from `operatorStack` to `outputQueue`.
        - Pop the left parenthesis `(` from `operatorStack` (do not add it to `outputQueue`).

5. After all tokens have been read:
    - While `operatorStack` is not empty:
        - Pop operators from `operatorStack` to `outputQueue`.

6. Return `outputQueue`, which now contains the postfix notation.

*/

MathOpNode* parseMathOp(Token* head, SymbolTable* table, int length) {
    //...
}

ASTNode* parseTokens(Token* head) {
    //...
}