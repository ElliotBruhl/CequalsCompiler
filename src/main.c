#include "tokenizer.h"
#include "varTable.h"
#include "funcTable.h"
#include "parser.h"
#include "codeGen.h"
#include <string.h>

int main(int argc, char** argv) {
    //COMPILING
    printf("\033[0;34mCompiling...0/6\033[0m\n");
    if (argc < 3) {
        printf("\033[0;31mFATAL ERROR: invalid number of arguments. Usage: {compiler file name (.exe)} {file to compile (.ceq)} {output file (.asm)} {optional flags (-printf)}\033[0m\n");
        return -1;
    }

    //READ FILE
    printf("\033[0;34mReading File...1/6\033[0m\n");
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to read input file\033[0m\n");
        return -1;
    }

    //TOKENIZE
    printf("\033[0;34mTokenizing...2/6\033[0m\n");
    Token *tokens = tokenizer(file); //closes file
    if (tokens == NULL) {
        printf("\033[0;31mFATAL ERROR: tokenizer failed\033[0m\n");
        return -2;
    }

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
    //standard library function calls
    bool usePrintf = false;
    bool useScanf = false;
    bool useMalloc = false;
    bool useFree = false;
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i], "-printf")) //args are address of format string and value
            usePrintf = true;
        else if (!strcmp(argv[i], "-scanf")) //args are address of format string and memory address to store value
            useScanf = true;
        else if (!strcmp(argv[i], "-malloc")) //arg is size in bytes
            useMalloc = true;
        else if (!strcmp(argv[i], "-free")) //arg is memory address
            useFree = true;
    }
    if (usePrintf) {
        printf("\033[0;34mIncluding printf...\033[0m\n");
        FuncEntry* printfEntry = pushFuncEntry(funcTable, "printf", 2);
        if (printfEntry == NULL) {
            printf("\033[0;31mFATAL ERROR: failed to add printf to funcTable\033[0m\n");
            return -4;
        }
    }
    if (useScanf) {
        printf("\033[0;34mIncluding scanf...\033[0m\n");
        FuncEntry* scanfEntry = pushFuncEntry(funcTable, "scanf", 2);
        if (scanfEntry == NULL) {
            printf("\033[0;31mFATAL ERROR: failed to add scanf to funcTable\033[0m\n");
            return -4;
        }
    }
    if (useMalloc) {
        printf("\033[0;34mIncluding malloc...\033[0m\n");
        FuncEntry* mallocEntry = pushFuncEntry(funcTable, "malloc", 1);
        if (mallocEntry == NULL) {
            printf("\033[0;31mFATAL ERROR: failed to add malloc to funcTable\033[0m\n");
            return -4;
        }
    }
    if (useFree) {
        printf("\033[0;34mIncluding free...\033[0m\n");
        FuncEntry* freeEntry = pushFuncEntry(funcTable, "free", 1);
        if (freeEntry == NULL) {
            printf("\033[0;31mFATAL ERROR: failed to add free to funcTable\033[0m\n");
            return -4;
        }
    }
    //parse tokens to AST
    ASTNode *AST = parseTokens(tokens, true, varTable, funcTable);
    if (AST == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to parse tokens\033[0m\n");
        return -5;
    }
    //free resources
    freeTokens(tokens);
    freeVarTable(varTable);
    freeFuncTable(funcTable);

    //CODE GENERATION
    printf("\033[0;34mCode Generation...5/6\033[0m\n");
    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        printf("\033[0;31mFATAL ERROR: failed to open output file\033[0m\n");
        fclose(output);
        return -6;
    }
    if (!codeGen(output, AST, usePrintf, useScanf, useMalloc, useFree)) {
        printf("\033[0;31mFATAL ERROR: failed to generate code\033[0m\n");
        fclose(output);
        return -7;
    }
    
    //CLEANUP
    printf("\033[0;34mCleaning Up...6/6\033[0m\n");
    fclose(output);
    freeASTNodes(AST);

    //COMPLETED
    printf("\033[0;32mProgram compiled without interupt.\033[0m\n");
    return 0;
}