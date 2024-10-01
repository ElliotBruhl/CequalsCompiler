#include "tokenizer.h"
#include "parser.h"
#include "identifierHashTable.h"
#include <stdio.h>

int main() {
    //TOKENIZE
    FILE *file = fopen("text.txt", "r");
    if (file == NULL) {
        printf("FATAL ERROR: failed to read input file\n");
        return -1;
    }
    Token *tokens = tokenizer(file);
    if (tokens == NULL) {
        printf("FATAL ERROR: tokenizer failed\n");
        freeTokens(tokens);
        return -2;
    }
    printTokens(tokens);

    //PARSE
    HashTable* table = createTable();
    ASTNode* ASTs = parseTokens(tokens); //finish other AST types
    if (ASTs == NULL) {
        printf("FATAL ERROR: parser failed\n");
        freeHashTable(table);
        //freeASTNodes(ASTs); segfaults
        return -3;
    }
    printASTs(ASTs);

    //WRITE IR (LLVM)
    
    //CLEANUP
    freeHashTable(table);
    //freeASTNodes(ASTs); //segfaults
    freeTokens(tokens);
    return 0;
}
/*
terminal: ctrl + shift + C
configure project: cmake -S . -B build
build and run: cmake --build build --target run
*/