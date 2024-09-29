#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <stdio.h>
#define MAX_BUFFER_SIZE 256

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_SEPERATOR
} TokenType;

typedef struct Token {
    int lineNum;
    TokenType tokenType;
    char* value;
    struct Token* prevToken;
    struct Token* nextToken;
} Token;

Token* tokenizer(FILE *file);
void printTokens(Token *head);
void freeTokens(Token *head);

#endif