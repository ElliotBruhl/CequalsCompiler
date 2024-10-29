#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdbool.h>

typedef struct {
    char* name;
    int numParams; //-1 for vars/arrays
} Entry;

typedef struct {
    int entryCount;
    Entry* entries;
    int entryCapacity; //dynamic re-sizing (default 10) - doubles when full
} Scope;

typedef struct {
    int scopeCount;
    Scope* scopes;
    int scopeCapacity; //dynamic re-sizing (default 10) - doubles when full
} SymbolTable;

SymbolTable* createTable();
bool pushScope(SymbolTable* table);
void popScope(SymbolTable* table);
bool pushEntry(SymbolTable* table, char* name, int numParams);
Entry* lookup(SymbolTable* table, char* name, int numParams);
void freeTable(SymbolTable* table);

#endif