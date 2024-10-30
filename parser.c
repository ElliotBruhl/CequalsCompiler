#include "parser.h"
#include "symbolTable.h"
#include <stdlib.h>

void freeASTNodes(ASTNode* head) {
    //...
}
void printASTs(ASTNode* head) {
    //...
}

int lowestPrecedence(Token* start, int length) { //first 2 bytes are precedence and last 2 bytes are operator
    if (sizeof(int) != 4 || sizeof(short) != 2) {
        printf("\033[1;31mError: int is not 4 bytes or short is not 2 bytes\033[0m\n");
        return -1; //precendence will never take up full 2 bytes (so return value will never be negative)
    }
    short lowest = -1;
    for (int i = 0; i < length; i++) {
        if (start->tokenType == TOKEN_OPERATOR) {
            //determine precedence
        }
    }
}
MathOpNode* parseMathOp(Token* head, SymbolTable* table, int length) {
    //look for lowest precedence operator (within range)
    //determine left and right types (mathOp then num/var/funcRet)
    //create the children nodes
    //if child is mathOp (recurse)


}

ASTNode* parseTokens(Token* head) {
    //...
}