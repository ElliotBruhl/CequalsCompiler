#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include "parser.h"

int main() {
    //COMPILING
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
        return -2;
    }
    printTokens(tokens); //DEBUG (temp)
        //CREATE SYMBOL TABLES
    printf("\033[0;34mCreating Symbol Tables...3/6\033[0m\n");
    VarTable *varTable = createVarTable();
    if (varTable == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create varTable\033[0m\n");
        return -3;
    }
    FuncTable *funcTable = createFuncTable();
    if (funcTable == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to create funcTable\033[0m\n");
        return -4;
    }
        //PARSE
    printf("\033[0;34mParsing...4/6\033[0m\n");
    ASTNode *AST = parseTokens(tokens, false, varTable, funcTable);
    if (AST == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to parse tokens\033[0m\n");
        return -5;
    }
    printASTs(AST); //DEBUG (temp)
    freeTokens(tokens);
        //CODE GENERATION
    printf("\033[0;34mCode Generation...5/6\033[0m\n");
    //...
    freeASTNodes(AST);
        //CLEANUP
    printf("\033[0;34mCleaning Up...6/6\033[0m\n");
    freeVarTable(varTable);
    freeFuncTable(funcTable);

    //COMPLETED
    printf("\033[0;32mProgram compiled without interupt.\033[0m\n");
    return 0;
}