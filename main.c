#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include "parser.h"

int main() {
    printf("\033[0;34mCompiling...\033[0m\n");
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

    printTokens(tokens);

        //CREATE SYMBOL TABLES
    VarTable *varTable = createVarTable();
    if (varTable == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create varTable\033[0m\n");
        freeTokens(tokens);
        return -3;
    }
    FuncTable *funcTable = createFuncTable();
    if (funcTable == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create funcTable\033[0m\n");
        freeTokens(tokens);
        return -4;
    }

    pushVarScope(varTable); //push global scope
    pushVarEntry(varTable, "a");
    pushVarEntry(varTable, "b");
    pushVarEntry(varTable, "c");
    pushFuncEntry(funcTable, "f1", 0);
    pushFuncEntry(funcTable, "f2", 1);
    pushFuncEntry(funcTable, "f3", 2);

            //PARSE
    Token* current = tokens->nextToken;
    while (current != NULL && current->nextToken != NULL) {
        if (current->nextToken->value[0] == ';') {
            parseMathOp(tokens, current, varTable, funcTable);
            tokens = current->nextToken->nextToken; //after semicolon
            current = tokens;
        }
        else
            current = current->nextToken;
    }

        //CLEANUP
    freeTokens(tokens);
    freeVarTable(varTable);
    freeFuncTable(funcTable);

    printf("\033[0;32mProgram compiled without interupt.\033[0m\n");
    return 0;
}