#ifndef VARTABLE_H
#define VARTABLE_H
#include <stdbool.h>

typedef struct { //these get referenced by ValueNodes in the AST with a new pointer and persist after the varTable is freed
    char* name;
    int stackOffset;
} VarEntry;

typedef struct {
    int entryCount;
    VarEntry** entries;
    int entryCapacity; //dynamic re-sizing (default 10) - doubles when full
    bool isParamScope; //true if this scope is for function parameters (positive offsets)
} VarScope;

typedef struct { //each function declaration gets its own table
    int scopeCount;
    VarScope** scopes;
    int scopeCapacity; //dynamic re-sizing (default 10) - doubles when full
} VarTable;

VarTable* createVarTable();
bool pushVarScope(VarTable* table, bool paramScope);
void popVarScope(VarTable* table);
VarEntry* pushVarEntry(VarTable* table, char* name);
VarEntry* varLookup(VarTable* table, char* name);
void freeVarTable(VarTable* table);

#endif