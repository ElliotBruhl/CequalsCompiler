#include "varTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define DEFAULT_CAPACITY 10
#define RESIZE_FACTOR 2

VarTable* createVarTable() {
    VarTable* table = malloc(sizeof(VarTable));
    if (table == NULL) {
        printf("\033[1;31mMalloc failed in createVarTable.\033[0m\n");
        return NULL;
    }
    table->scopeCount = 0;
    table->scopeCapacity = DEFAULT_CAPACITY;
    table->scopes = malloc(sizeof(VarScope*) * DEFAULT_CAPACITY);
    if (table->scopes == NULL) {
        printf("\033[1;31mMalloc failed in createVarTable.\033[0m\n");
        free(table);
        return NULL;
    }
    return table;
}
void popVarScope(VarTable* table) {
    if (table == NULL) return;
    if (table->scopeCount > 0) {
        free(table->scopes[table->scopeCount - 1]->entries);
        table->scopeCount--;
    }
}
void freeVarTable(VarTable* table) { //the entries are all freed after codegen (they get referenced by a new pointer during parsing)
    if (table == NULL) return;
    while (table->scopeCount > 0) {
        popVarScope(table);
    }
    free(table->scopes);
    free(table);
}
bool pushVarScope(VarTable* table) {
    if (table == NULL) return false;
    if (table->scopeCount == table->scopeCapacity) { //resize if full
        table->scopeCapacity *= RESIZE_FACTOR;
        table->scopes = realloc(table->scopes, sizeof(VarScope*) * table->scopeCapacity);
        if (table->scopes == NULL){
            printf("\033[1;31mRealloc failed in pushScope.\033[0m\n");
            return false;
        }
    }
    table->scopes[table->scopeCount]->entryCount = 0;
    table->scopes[table->scopeCount]->entryCapacity = DEFAULT_CAPACITY;
    table->scopes[table->scopeCount]->entries = malloc(sizeof(VarEntry*) * DEFAULT_CAPACITY);
    if (table->scopes[table->scopeCount]->entries == NULL) {
        printf("\033[1;31mMalloc failed in pushScope.\033[0m\n");
        return false;
    }
    table->scopeCount++;
    return true;
}
bool pushVarEntry(VarTable* table, char* name) {
    if (table == NULL || name == NULL) return false;
    if (table->scopeCount == 0) { //no scope to push to 
        printf("\033[1;31mNo scope to push to in pushEntry.\033[0m\n");
        return false;
    }
    VarScope* curScope = table->scopes[table->scopeCount - 1];
    if (curScope->entryCount == curScope->entryCapacity) { //resize if full
        curScope->entryCapacity *= RESIZE_FACTOR;;
        curScope->entries = realloc(curScope->entries, sizeof(VarEntry*) * curScope->entryCapacity);
        if (curScope->entries == NULL) {
            printf("\033[1;31mRealloc failed in pushEntry.\033[0m\n");
            return false;
        }
    }
    curScope->entries[curScope->entryCount]->name = strdup(name);
    if (curScope->entries[curScope->entryCount]->name == NULL) {
        printf("\033[1;31mStrdup failed in pushEntry.\033[0m\n");
        return false;
    }
    //calc offset
    curScope->entries[curScope->entryCount]->stackOffset = (table->scopes[table->scopeCount - 1]->entryCount) * 8 + 8;
    curScope->entryCount++;
    return true;
}
VarEntry* varLookup(VarTable* table, char* name) {
    if (table == NULL || name == NULL) return NULL;
    for (int i = table->scopeCount - 1; i >= 0; i--) {
        VarScope* curScope = table->scopes[i];
        for (int j = curScope->entryCount - 1; j >= 0; j--) {
            if (!strcmp(curScope->entries[j]->name, name)) {
                return curScope->entries[j];
            }
        }
    }
    return NULL;
}