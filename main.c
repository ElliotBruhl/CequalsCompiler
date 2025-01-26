#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include "parser.h"

int main() {
    printf("\033[0;34mCompiling...0/6\033[0m\n");
        //READ FILE
    printf("\033[0;34mReading File...1/6\033[0m\n");
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to read input file\033[0m\n");
        return -1;
    }

        //TOKENIZE
    printf("\033[0;34mTokenizing...2/6\033[0m\n");
    Token *tokens = tokenizer(file);
    if (tokens == NULL) {
        printf("\033[0;31mFATAL ERROR: tokenizer failed\033[0m\n");
        freeTokens(tokens);
        return -2;
    }
    printTokens(tokens); //TEMP

        //CREATE SYMBOL TABLES
    printf("\033[0;34mCreating Symbol Tables...3/6\033[0m\n");
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

        //PARSE
    printf("\033[0;34mParsing...4/6\033[0m\n");
    pushVarScope(varTable); //TEMP
    pushVarEntry(varTable, "a");
    pushVarEntry(varTable, "b");
    pushVarEntry(varTable, "c");
    pushFuncEntry(funcTable, "f1", 0);
    pushFuncEntry(funcTable, "f2", 1);
    pushFuncEntry(funcTable, "f3", 2); 
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
    freeTokens(tokens);

        //CODE GENERATION
    printf("\033[0;34mCode Generation...5/6\033[0m\n");

        //CLEANUP
    printf("\033[0;34mCleaning Up...6/6\033[0m\n");
    freeVarTable(varTable);
    freeFuncTable(funcTable);
    //freeASTNodes(AST);

    printf("\033[0;32mProgram compiled without interupt.\033[0m\n");
    return 0;
}