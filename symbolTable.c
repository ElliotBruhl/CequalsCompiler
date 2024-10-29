#include "symbolTable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define DEFAULT_CAPACITY 10
#define RESIZE_FACTOR 2

SymbolTable *createTable() {
    SymbolTable *table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (table == NULL) {
        printf("\033[1;31mMalloc failed in createSymbolTable.\033[0m\n");
        return NULL;
    }
    table->scopeCount = 0;
    table->scopeCapacity = DEFAULT_CAPACITY;
    table->scopes = malloc(sizeof(Scope) * DEFAULT_CAPACITY);
    if (table->scopes == NULL) {
        printf("\033[1;31mMalloc failed in createSymbolTable.\033[0m\n");
        free(table);
        return NULL;
    }
    return table;
}

void freeScope(Scope *scope) {
    if (scope == NULL) return;
    for (int i = 0; i < scope->entryCount; i++) {
        if (scope->entries[i].name != NULL)
            free(scope->entries[i].name);
    }
    if (scope->entries != NULL)
        free(scope->entries);
}
void freeTable(SymbolTable *table) {
    if (table == NULL) return;
    for (int i = 0; i < table->scopeCount; i++) {
        if (table->scopes[i].entries != NULL)
            freeScope(&table->scopes[i]);
    }
    free(table->scopes);
    free(table);
}

bool pushScope(SymbolTable *table) {
    if (table->scopeCount == table->scopeCapacity) { //resize if full
        table->scopeCapacity *= RESIZE_FACTOR;
        table->scopes = realloc(table->scopes, sizeof(Scope) * table->scopeCapacity);
        if (table->scopes == NULL){
            printf("\033[1;31mRealloc failed in pushScope.\033[0m\n");
            return false;
        }
    }
    
    table->scopes[table->scopeCount].entryCount = 0;
    table->scopes[table->scopeCount].entryCapacity = DEFAULT_CAPACITY;
    table->scopes[table->scopeCount].entries = (Entry*)malloc(sizeof(Entry) * DEFAULT_CAPACITY);
    if (table->scopes[table->scopeCount].entries == NULL) {
        printf("\033[1;31mMalloc failed in pushScope.\033[0m\n");
        return false;
    }
    table->scopeCount++;
    return true;
}
void popScope(SymbolTable *table) {
    if (table->scopeCount > 0) {
        freeScope(&table->scopes[table->scopeCount - 1]);
        table->scopeCount--;
    }
}
bool pushEntry(SymbolTable *table, char *name, int numParams) {
    if (table->scopeCount == 0) { //no scope to push to 
        printf("\033[1;31mNo scope to push to in pushEntry.\033[0m\n");
        return false;
    }

    Scope *curScope = &table->scopes[table->scopeCount - 1];

    if (curScope->entryCount == curScope->entryCapacity) { //resize if full
        curScope->entryCapacity *= RESIZE_FACTOR;;
        curScope->entries = realloc(curScope->entries, sizeof(Entry) * curScope->entryCapacity);
        if (curScope->entries == NULL) {
            printf("\033[1;31mRealloc failed in pushEntry.\033[0m\n");
            return false;
        }
    }

    curScope->entries[curScope->entryCount].name = strdup(name);
    if (curScope->entries[curScope->entryCount].name == NULL) {
        printf("\033[1;31mStrdup failed in pushEntry.\033[0m\n");
        return false;
    }
    curScope->entries[curScope->entryCount].numParams = numParams;
    curScope->entryCount++;
    return true;
}
Entry *lookup(SymbolTable *table, char *name, int numParams) {
    for (int i = table->scopeCount - 1; i >= 0; i--) {
        Scope *curScope = &table->scopes[i];
        for (int j = curScope->entryCount - 1; j >= 0; j--) {
            if (strcmp(curScope->entries[j].name, name) == 0 && curScope->entries[j].numParams == numParams) {
                return &curScope->entries[j];
            }
        }
    }
    return NULL;
}