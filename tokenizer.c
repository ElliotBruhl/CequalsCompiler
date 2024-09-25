#include "tokenizer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

Token *createToken(int line, int type, char *valStart, int valLen, Token *prev) {
    Token *newToken = (Token *)malloc(sizeof(Token));
    if (newToken == NULL)
        exit(1);
    
    newToken->lineNum = line;
    newToken->tokenType = type;

    newToken->value = (char *)malloc(valLen);
    if (newToken->value == NULL)
        exit(1);
    strncpy(newToken->value, valStart, valLen);
    newToken->value[valLen] = '\0';

    newToken->prevToken = prev;
    newToken->nextToken = NULL;

    if (prev != NULL) { // update previous to point at this token
        prev->nextToken = newToken;
    }

    return newToken;
}
void printTokens(Token *head) {
    Token *current = head;
    while (current != NULL) {
        printf("Line: %d, Token type: %d, Value: %s\n", current->lineNum, current->tokenType, current->value);
        current = current->nextToken;
    }
}
void freeTokens(Token *head) {
    Token *current = head;
    while (current != NULL) {
        Token *next = current->nextToken;
        free(current->value);
        free(current);
        current = next;
    }
}

int getOperatorLength(char c, char cNext) {
    /* non operator -> 0, one char op -> 1, two char op -> 2
    oneCharOps can be made into two char ops by adding "=". Special case: "&"
    and "|" can also have "&" or "|" appened.
    */
    if ((c == '&' && cNext == '&') || (c == '|' && cNext == '|')) return 2; //special case

    char oneCharOps[] = {'+', '-', '*', '/', '%', '=', '<', '>', '!', '&', '|', '~', '^'};
    const int OPS_LEN = 13;

    for (int i = 0; i < OPS_LEN; i++) {
        if (c == oneCharOps[i]) { // one char op found
            if (cNext == '=') { // two char op
                return 2;
            }
            return 1;
        }
    }
    return 0; // not operator
}
int getKeywordLength(char *c) {
    /* loops through each defined keyword until mismatch in chars is found.
    If no mismatch, length of keyword is returned. If none found, 0 returned.
    note: spaces after keywords that take args (all but continue and break) are required. 
        this means continue and break will be considered keywords by tokenizer even if found as break123 for ex, but all others will not
    */
    char *keywords[] = {"var ", "func ", "while ", "if ", "else ", "return ", "continue", "break"}; //
    const int KEY_LEN = 8;

    for (int i = 0; i < KEY_LEN; i++) {
        int j = 0;
        for (; keywords[i][j] != '\0'; j++) {
            if (keywords[i][j] != *(c+j)) {
                break;
            }
        }
        if (keywords[i][j] == '\0') {
            return j;
        }
    }
    return 0;
}

Token *tokenizer(FILE *file) {
    char buffer[MAX_BUFFER_SIZE];
    int lineNum = 1;
    Token *head = NULL;
    Token *prevToken = NULL;
    int opLen;

    while (fgets(buffer, sizeof(buffer), file) != NULL) { // each line is stored in buffer

        if ((strlen(buffer) == MAX_BUFFER_SIZE - 1) && (buffer[MAX_BUFFER_SIZE - 2] != '\n')) { // set limit on line size to prevent bugs when new buffer
            printf("Error: buffer overflow - line %d over length 255 chars\n", lineNum);
            fclose(file);
            freeTokens(head);
            exit(3);
        }

        // loop over each char in buffer
        for (int i = 0; i < MAX_BUFFER_SIZE && buffer[i] != '\0'; i++) {

            int tokenType = -1;
            int end = i; // keeps track of end for multi-char tokens

            if (isspace(buffer[i]))
                continue; // skip to next char for whitespace
            else if (buffer[i] == '#')
                break; // skip to next line for comment
            else if (isalpha(buffer[i])) { // identifiers and keywords
                int keywordLen = getKeywordLength(&buffer[i]);
                if (keywordLen != 0) { // keyword
                    tokenType = 3;
                    end += keywordLen - 1;
                }
                else { // identifier
                    tokenType = 0;
                    while (isalnum(buffer[end + 1]))
                        end++;
                }
            }
            else if (isdigit(buffer[i]) || ((buffer[i] == '-' || buffer[i] == '+') && isdigit(buffer[i+1]))) { // numbers, can start with '+' or '-'
                tokenType = 1;
                while (isdigit(buffer[end + 1]))
                    end++;
            }
            else if (buffer[i] == '(' || buffer[i] == ')' || // seperators
                    buffer[i] == '{' || buffer[i] == '}' || buffer[i] == ';') {
                tokenType = 4;
            }
            else if ((opLen = getOperatorLength(buffer[i], buffer[i + 1])) != 0) { // operator
                tokenType = 2;
                end += opLen - 1;
            }
            else { // unknown char
                printf("Unknown char %c on line %d, column %d", buffer[i], lineNum, i + 1);
                fclose(file);
                freeTokens(head);
                exit(4);
            }

            // create token and put into linked list
            Token *newToken = createToken(lineNum, tokenType, &buffer[i], end - i + 1, prevToken);
            if (head == NULL)
                head = newToken;
            prevToken = newToken;

            // update i for multi-char tokens
            i = end;
        }
        lineNum++;
    }

    fclose(file);

    return head;
}