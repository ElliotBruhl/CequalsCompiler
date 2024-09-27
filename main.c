#include "tokenizer.h"
#include "parser.h"
#include "identifierHashTable.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    
    FILE *file = fopen("text.txt", "r");
    if (file == NULL)
        exit(2);
    Token *tokens = tokenizer(file);
    printTokens(tokens);
    freeTokens(tokens);

    HashTable* table = createTable();
    freeHashTable(table);

    exit(0);
}
//build command: gcc main.c tokenizer.c parser.c -o main