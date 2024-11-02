#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token* createToken(int line, TokenType type, char* valStart, int valLen, Token* prev) {
    Token* newToken = (Token*)malloc(sizeof(Token));
    if (newToken == NULL) {
        printf("\033[1;31mMalloc Failed in createToken\033[0m\n");
        return NULL;
    }
    
    newToken->lineNum = line;
    newToken->tokenType = type;

    newToken->value = (char*)malloc(valLen);
    if (newToken->value == NULL) {
        printf("\033[1;31mMalloc Failed in createToken\033[0m\n");
        return NULL;
    }
    strncpy(newToken->value, valStart, valLen);
    newToken->value[valLen] = '\0';

    newToken->prevToken = prev;
    newToken->nextToken = NULL;

    if (prev != NULL) { // update previous to point at this token
        prev->nextToken = newToken;
    }

    return newToken;
}
void printTokens(Token* head) {
    Token* current = head;
    while (current != NULL) {
        printf("Line: %d, Token type: %d, Value: %s\n", current->lineNum, current->tokenType, current->value);
        current = current->nextToken;
    }
}
void freeTokens(Token* head) {
    Token* current = head;
    while (current != NULL) {
        Token* next = current->nextToken;
        free(current->value);
        free(current);
        current = next;
    }
}

int getOperatorLength(char c, char cNext) {
    switch (c) {
        case '=':
            if (cNext == '=') return 2;
        case '<':
            if (cNext == '=' || cNext == '<') return 2;
        case '>':
            if (cNext == '=' || cNext == '>') return 2;
        case '!':
            if (cNext == '=') return 2;
        case '&':
            if (cNext == ' ' || cNext == '&') return 2; // & followed by space is bit and or followed by & is &&, but & followed by char is ref
        case '|':
            if (cNext == '|') return 2;
        case '*':
            if (cNext == ' ') return 2; // * followed by space is mul, but * followed by another char is pointer deref
        case '+':
        case '-':
        case '/':
        case '%':
        case '~':
        case '^':
            return 1;
        default:
            return 0;
    }
}
int getKeywordLength(char* c) {
    char* keywords[] = {"var", "func", "return", "while", "continue", "break", "if", "else"};
    const int KEYWORDS_LEN = 8;

    for (int i = 0; i < KEYWORDS_LEN; i++) {
        int j = 0;
        for (; keywords[i][j] != '\0'; j++) {
            if (keywords[i][j] != *(c+j)) {
                break;
            }
        }
        if (keywords[i][j] == '\0' && !isalnum(*(c+j))) {
            return j;
        }
    }
    return 0;
}

Token* tokenizer(FILE* file) {
    char buffer[MAX_BUFFER_SIZE];
    int lineNum = 1;
    Token* head = NULL;
    Token* prevToken = NULL;
    int opLen;

    while (fgets(buffer, sizeof(buffer), file) != NULL) { // each line is stored in buffer

        if ((strlen(buffer) == MAX_BUFFER_SIZE - 1) && (buffer[MAX_BUFFER_SIZE - 2] != '\n')) { // set limit on line size to prevent bugs on split tokens
            printf("\033[1;31mError: line %d over length 255 chars\033[0m\n", lineNum);
            fclose(file);
            return NULL;
        }

        // loop over each char in buffer
        for (int i = 0; i < MAX_BUFFER_SIZE && buffer[i] != '\0'; i++) {

            int tokenType = -1;
            int end = i; // keeps track of end for multi-char tokens
            
            if (buffer[i] == '#') break; // skip to next line for comment
            else if (isspace(buffer[i])) continue; // skip to next char for whitespace
            else if (isalpha(buffer[i])) { // identifiers and keywords
                int keywordLen = getKeywordLength(&buffer[i]);
                if (keywordLen != 0) { // keyword
                    tokenType = TOKEN_KEYWORD;
                    end += keywordLen - 1;
                }
                else { // identifier
                    tokenType = TOKEN_IDENTIFIER;
                    while (isalnum(buffer[end + 1]))
                        end++;
                }
            }
            else if (isdigit(buffer[i]) || ((buffer[i] == '-' || buffer[i] == '+') && isdigit(buffer[i+1]))) { // numbers, can start with '+' or '-'
                tokenType = TOKEN_NUMBER;
                while (isdigit(buffer[end + 1]))
                    end++;
            }
            else if (buffer[i] == '(' || buffer[i] == ')' || buffer[i] == ',' || // seperators
                    buffer[i] == '{' || buffer[i] == '}' || buffer[i] == ';') {
                tokenType = TOKEN_SEPERATOR;
            }
            else if ((opLen = getOperatorLength(buffer[i], buffer[i + 1])) != 0) { // operator
                tokenType = TOKEN_OPERATOR;
                end += opLen - 1;
            }
            else { // unknown char
                printf("\033[1;31mUnknown char %c on line %d, column %d\033[0m\n", buffer[i], lineNum, i + 1);
                fclose(file);
                return NULL;
            }

            // create token and put into linked list
            Token* newToken = createToken(lineNum, tokenType, &buffer[i], end - i + 1, prevToken);
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