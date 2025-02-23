#ifndef FUNCTABLE_H
#define FUNCTABLE_H
#include <stdbool.h>

typedef struct {
    char* name;
    int paramCount;
} FuncEntry;

typedef struct {
    int entryCount;
    FuncEntry** entries;
    int entryCapacity; //dynamic re-sizing (default 5) - doubles when full
} FuncTable;

FuncTable* createFuncTable();
bool pushFuncEntry(FuncTable* table, char* name, int paramCount);
FuncEntry* funcLookup(FuncTable* table, char* name, int paramCount);
void freeFuncTable(FuncTable* table);

#endif