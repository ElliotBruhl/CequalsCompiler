#ifndef VARTABLE_H
#define VARTABLE_H
#include <stdbool.h>

typedef struct {
    char* name;
    int stackOffset; //stack offset for vars (from basePtr in the scope) - first one should be 8, then 16, etc.
} VarEntry;

typedef struct {
    int entryCount;
    VarEntry** entries;
    int entryCapacity; //dynamic re-sizing (default 10) - doubles when full
} VarScope;

typedef struct {
    int scopeCount;
    VarScope** scopes;
    int scopeCapacity; //dynamic re-sizing (default 10) - doubles when full
} VarTable;

VarTable* createVarTable();
bool pushVarScope(VarTable* table);
void popVarScope(VarTable* table);
bool pushVarEntry(VarTable* table, char* name);
VarEntry* varLookup(VarTable* table, char* name);
void freeVarTable(VarTable* table);

#endif