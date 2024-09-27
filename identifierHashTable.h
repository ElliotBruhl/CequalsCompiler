#ifndef IDENTIFIERHASHTABLE_H
#define IDENTIFIERHASHTABLE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define TABLE_SIZE 509 //probably overkill - use prime to help distribution

typedef struct {
    //add neccesary info later
} FuncInfo;
typedef struct Entry {
    char* name;
    bool isVar;
    union {
        int varValue;
        FuncInfo* funcInfo;
    } value;
} Entry;
typedef struct {
    Entry* entries[TABLE_SIZE];
} HashTable;

unsigned int hash(char* str) { //DJB2 hash algorithm
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % TABLE_SIZE;
}
HashTable* createTable() {
    HashTable* table = malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
    return table;
}
void insertVar(HashTable* table, char* nameToStore, int valToStore) {
    unsigned int index = hash(nameToStore);

    Entry* entry = malloc(sizeof(Entry));
    entry->isVar = true;
    entry->name = strdup(nameToStore);
    entry->value.varValue = valToStore;

    table->entries[index] = entry;
}
void insertFunc(HashTable* table, char* nameToStore) {
    unsigned int index = hash(nameToStore);

    Entry* entry = malloc(sizeof(Entry));
    entry->isVar = false;
    entry->name = strdup(nameToStore);
    //create FuncInfo struct with neccesary info and store

    table->entries[index] = entry;
}
Entry* lookup(HashTable* table, char* nameToLookup) {
    unsigned int index = hash(nameToLookup);
    Entry* entry = table->entries[index];

    if (entry == NULL) //no value
        return NULL; 
    if (strcmp(entry->name, nameToLookup) == 0) //value matches
        return entry;
    else //collision
        exit(5);
}
void freeHashTable(HashTable* table) {
    if (!table) return;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->entries[i] == NULL) continue;
        free(table->entries[i]->name);
        if (!table->entries[i]->isVar) {
            //free FuncInfo struct
        }
        free(table->entries[i]);
    }
    free(table);
}

#endif