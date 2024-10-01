#ifndef IDENTIFIERHASHTABLE_H
#define IDENTIFIERHASHTABLE_H
#include <stdbool.h>

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

unsigned int hash(char* str);
HashTable* createTable();
void insertVar(HashTable* table, char* nameToStore, int valToStore, unsigned int hashVal);
void insertFunc(HashTable* table, char* nameToStore);
Entry* lookup(HashTable* table, char* nameToLookup);
void freeHashTable(HashTable* table);

#endif