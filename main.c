#include "tokenizer.h"
#include "parser.h"
#include "symbolTable.h"
#include <stdio.h>

int main() {
        //READ FILE
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to read input file\033[0m\n");
        return -1;
    }

        //TOKENIZE
    Token *tokens = tokenizer(file);
    if (tokens == NULL) {
        printf("\033[0;31mFATAL ERROR: tokenizer failed\033[0m\n");
        freeTokens(tokens);
        return -2;
    }
    printTokens(tokens); //for debugging (temp)

        //CREATE SYMBOL TABLE
    SymbolTable *table = createTable();
    if (table == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create symbol table\033[0m\n");
        freeTokens(tokens);
        return -3;
    }
    
            //PARSE
    parseMathOp(tokens, table, 7);

        //CLEANUP
    freeTokens(tokens);
    freeTable(table);

    printf("\033[0;32mSUCCESS: compiled without fatal errors\033[0m\n");
    return 0;
}