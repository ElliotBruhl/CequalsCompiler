#include "funcTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define DEFAULT_CAPACITY 10
#define RESIZE_FACTOR 2

FuncTable* createFuncTable() {
    FuncTable* table = malloc(sizeof(FuncTable));
    if (table == NULL) {
        printf("\033[1;31mMalloc failed in createFuncTable.\033[0m\n");
        return NULL;
    }
    table->entryCount = 0;
    table->entryCapacity = DEFAULT_CAPACITY;
    table->entries = malloc(sizeof(FuncEntry*) * DEFAULT_CAPACITY);
    if (table->entries == NULL) {
        printf("\033[1;31mMalloc failed in createFuncTable.\033[0m\n");
        free(table);
        return NULL;
    }
    return table;
}
FuncEntry* pushFuncEntry(FuncTable* table, char* name, int paramCount) {
    if (name == NULL || table == NULL) return NULL;
    if (table->entryCount == table->entryCapacity) { //resize if full
        table->entryCapacity *= RESIZE_FACTOR;
        table->entries = realloc(table->entries, sizeof(FuncEntry*) * table->entryCapacity);
        if (table->entries == NULL){
            printf("\033[1;31mRealloc failed in pushEntry.\033[0m\n");
            return NULL;
        }
    }
    table->entries[table->entryCount] = malloc(sizeof(FuncEntry));
    if (table->entries[table->entryCount] == NULL) {
        printf("\033[1;31mMalloc failed in pushEntry.\033[0m\n");
        return NULL;
    }
    table->entries[table->entryCount]->name = strdup(name);
    if (table->entries[table->entryCount]->name == NULL) {
        printf("\033[1;31mStrdup failed in pushEntry.\033[0m\n");
        return NULL;
    }
    table->entries[table->entryCount]->paramCount = paramCount;
    table->entryCount++;
    return table->entries[table->entryCount - 1];
}
FuncEntry* funcLookup(FuncTable* table, char* name, int paramCount) {
    if (name == NULL || table) return NULL;
    for (int i = 0; i < table->entryCount; i++) {
        if (!strcmp(table->entries[i]->name, name) && table->entries[i]->paramCount == paramCount)
            return table->entries[i];
    }
    return NULL;
}
void freeFuncTable(FuncTable *table) { //the entries are all freed after codegen (they get referenced by a new pointer during parsing)
    if (table == NULL) return;
    if (table->entries != NULL)
        free(table->entries);
    free(table);
}