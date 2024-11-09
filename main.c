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
    //ADD ERRORS FOR INVALID MATH OPERATIONS (they mostly pass now)
    //for test suites
    int i = 0;
    int j = 1;
    Token* opStart = tokens;
    for (Token *current = tokens; current != NULL; current = current->nextToken) {
        if (current->value[0] == ';') {
            printf("Parsing test case %d\n", j++);
            parseMathOp(opStart, table, i);
            opStart = current->nextToken;
            i = 0;
        }
        else {
            i++;
        }
    }

        //CLEANUP
    freeTokens(tokens);
    freeTable(table);

    printf("\033[0;32mSUCCESS: compiled without fatal errors\033[0m\n");
    return 0;
}