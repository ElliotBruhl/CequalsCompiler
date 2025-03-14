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
        return NULL;
    }
    return table;
}
void popVarScope(VarTable* table) { //varEntries are freed after codegen
    if (table == NULL) return;
    if (table->scopeCount > 0) {
        free(table->scopes[table->scopeCount - 1]->entries);
        table->scopeCount--;
    }
}
void freeVarTable(VarTable* table) { //VarEntries are freed after codegen
    if (table == NULL) return;
    while (table->scopeCount > 0) {
        free(table->scopes[table->scopeCount - 1]->entries);
        table->scopeCount--;
    }
    free(table->scopes);
    free(table);
}
bool pushVarScope(VarTable* table, bool paramScope) {
    if (table == NULL) return false;
    if (table->scopeCount == table->scopeCapacity) { //resize if full
        table->scopeCapacity *= RESIZE_FACTOR;
        table->scopes = realloc(table->scopes, sizeof(VarScope*) * table->scopeCapacity);
        if (table->scopes == NULL){
            printf("\033[1;31mRealloc failed in pushScope.\033[0m\n");
            return false;
        }
    }
    table->scopes[table->scopeCount] = malloc(sizeof(VarScope));
    if (table->scopes[table->scopeCount] == NULL) {
        printf("\033[1;31mMalloc failed in pushScope.\033[0m\n");
        return false;
    }
    table->scopes[table->scopeCount]->entryCount = 0;
    table->scopes[table->scopeCount]->entryCapacity = DEFAULT_CAPACITY;
    table->scopes[table->scopeCount]->isParamScope = paramScope;
    table->scopes[table->scopeCount]->entries = malloc(sizeof(VarEntry*) * DEFAULT_CAPACITY);
    if (table->scopes[table->scopeCount]->entries == NULL) {
        printf("\033[1;31mMalloc failed in pushScope.\033[0m\n");
        return false;
    }
    table->scopeCount++;
    return true;
}
VarEntry* pushVarEntry(VarTable* table, char* name) {
    if (table == NULL || name == NULL) return NULL;
    if (table->scopeCount == 0) { //no scope to push to
        printf("\033[1;31mNo scope to push to in pushEntry.\033[0m\n");
        return NULL;
    }
    VarScope* curScope = table->scopes[table->scopeCount - 1];
    if (curScope->entryCount == curScope->entryCapacity) { //resize if full
        curScope->entryCapacity *= RESIZE_FACTOR;;
        curScope->entries = realloc(curScope->entries, sizeof(VarEntry*) * curScope->entryCapacity);
        if (curScope->entries == NULL) {
            printf("\033[1;31mRealloc failed in pushEntry.\033[0m\n");
            return NULL;
        }
    }
    curScope->entries[curScope->entryCount] = malloc(sizeof(VarEntry));
    if (curScope->entries[curScope->entryCount] == NULL) {
        printf("\033[1;31mMalloc failed in pushEntry.\033[0m\n");
        return NULL;
    }
    curScope->entries[curScope->entryCount]->name = strdup(name);
    if (curScope->entries[curScope->entryCount]->name == NULL) {
        printf("\033[1;31mStrdup failed in pushEntry.\033[0m\n");
        return NULL;
    }
    //calc offset
    if (curScope->isParamScope) 
        curScope->entries[curScope->entryCount]->stackOffset = (curScope->entryCount + 2) * 8; //positive offset from base pointer and skip return address
    else
        curScope->entries[curScope->entryCount]->stackOffset = (curScope->entryCount + 1) * -8; //negative offset from base pointer
    curScope->entryCount++;
    return curScope->entries[curScope->entryCount - 1];
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