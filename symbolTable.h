#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdbool.h>

typedef struct {
    char* name;
    bool isVar; //true if variable, false if function
    union {
        long long value; //for variable
        int numParams; //for function
    } data;
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
bool pushEntry(SymbolTable* table, char* name, bool isVar, void* data);
Entry* lookup(SymbolTable* table, char* name, bool isVar);
void freeTable(SymbolTable* table);

#endif