#include <identifierHashTable.h>
#include <stdlib.h>
#include <string.h>

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
void insertVar(HashTable* table, char* nameToStore, int valToStore, unsigned int hashVal) {
    Entry* entry = malloc(sizeof(Entry));
    entry->isVar = true;
    entry->name = strdup(nameToStore);
    entry->value.varValue = valToStore;

    table->entries[hashVal] = entry;
}
void insertFunc(HashTable* table, char* nameToStore) {
    unsigned int index = hash(nameToStore);

    Entry* entry = malloc(sizeof(Entry));
    entry->isVar = false;
    entry->name = strdup(nameToStore);
    //create FuncInfo struct with neccesary info and store

    table->entries[index] = entry;
}
Entry* lookup(HashTable* table, char* nameToLookup) { //null return can mean hash collision or no value - make sure both are error states
    unsigned int index = hash(nameToLookup);
    Entry* entry = table->entries[index];

    if (entry == NULL) //no value
        return NULL; 
    if (strcmp(entry->name, nameToLookup) == 0) //value matches
        return entry;
    return NULL; //hash collision
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