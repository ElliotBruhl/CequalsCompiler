#include "tokenizer.h"
#include "parser.h"
#include "symbolTable.h"
#include <stdio.h>

int main() {
        //TOKENIZE
    FILE *file = fopen("text.txt", "r");
    if (file == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to read input file\033[0m\n");
        return -1;
    }
    Token *tokens = tokenizer(file);
    if (tokens == NULL) {
        printf("\033[0;31mFATAL ERROR: tokenizer failed\033[0m\n");
        freeTokens(tokens);
        return -2;
    }
    printTokens(tokens);

        //PARSE
    /*
    SymbolTable *table = createTable();
    if (table == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create symbol table\033[0m\n");
        freeTokens(tokens);
        return -3;
    }
    ASTNode *AST = parse(tokens, table);
    if (AST == NULL) {
        printf("\033[0;31mFATAL ERROR: parser failed\033[0m\n");
        freeTokens(tokens);
        freeTable(table);
        return -4;
    }
    //printAST(AST);

        //CodeGen (LLVM IR) 
    //todo
    */
        //CLEANUP
    freeTokens(tokens);
    //freeTable(table);
    return 0;
}