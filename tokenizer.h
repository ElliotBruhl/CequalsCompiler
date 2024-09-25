#include <stdio.h>
#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef struct Token {
    int lineNum;
    int tokenType;
    char *value;
    struct Token *prevToken;
    struct Token *nextToken;
} Token;

Token *tokenizer(FILE *file);
void printTokens(Token *head);
void freeTokens(Token *head);
int getOperatorLength(char c, char cNext);
int getKeywordLength(char *c);
Token *createToken(int lineNum, int type, char *valStart, int valLen, Token *prev);

#endif