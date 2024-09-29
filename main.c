#include "tokenizer.h"
#include "parser.h"
#include "identifierHashTable.h"
#include <stdio.h>

int main() {
    //TOKENIZE
    FILE *file = fopen("text.txt", "r");
    if (file == NULL)
        return 2;
    Token *tokens = tokenizer(file);
    if (tokens == NULL) {
        freeTokens(tokens);
        return 1;
    }
    printTokens(tokens);

    //PARSE
    HashTable* table = createTable();
    ASTNode* ASTs = parseTokens(tokens); //finish other AST types

    //WRITE IR (LLVM)
    
    //CLEANUP
    freeHashTable(table);
    freeASTNodes(ASTs); //still need to test this (after parseTokens is done)
    freeTokens(tokens);
    return 0;
}
/*
terminal: ctrl + shift + C
configure project: cmake -S . -B build
build and run: cmake --build build --target run
*/
