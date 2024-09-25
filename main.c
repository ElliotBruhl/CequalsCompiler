#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    
    FILE *file = fopen("text.txt", "r");
    if (file == NULL)
        exit(2);
    Token *tokens = tokenizer(file);

    printTokens(tokens);

    freeTokens(tokens);

    return 0;
}
//build command: gcc main.c tokenizer.c parser.c -o main