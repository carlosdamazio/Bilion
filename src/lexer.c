#include <bits/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void free_tokens(Token *tokens)
{
    Token *start = tokens;

    while (tokens->value != NULL)
        free(tokens++->value);

    tokens = start;
    free(tokens);
}

// This function lexes a line into an array of tokens, with type and value.
Token* lex(char *line)
{
    Token *tokens = malloc(sizeof(Token) * LEX_CAP);
    bool is_string = false;
    char delim_stack[10];
    char buff[LEX_CAP];
    buff[LEX_CAP-1] = '\0';
    int counter = 0;
    int token_counter = 0;
    int delim_tracker = 0;

    if (tokens == NULL) {
        fprintf(stderr, "[ERROR] Couldn't allocate memory for token array");
        return NULL;
    }

    for (size_t i = 0; i < strlen(line); i++) {
        // String literal check
        if (is_string && line[i] != '"') {
            buff[counter++] = line[i];
            continue;
        }
        
        // Ignore spaces
        if (isspace(line[i]))
            continue;
        
        // Keyword check
        if (isalpha(line[i])) {
            buff[counter++] = line[i];
            // see if identifier is a keyword, and reset it afterwards
            if (strcmp(buff, "exposed") == 0) {
                Token token;
                token.kind = TOK_PRINT;
                token.value = malloc(sizeof(char) * 1024);
                strncpy(token.value, buff, counter);
                buff[0] = '\0';
                counter = 0;
                tokens[token_counter++] = token;
            }
            continue;
        }
        
        // Delimiters check
        switch (line[i]) {
            case ';': {
                buff[counter++] = line[i];
                Token token;
                token.kind = TOK_END_EXPR_DELIM;
                token.value = malloc(sizeof(char) * 1024);
                strncpy(token.value, buff, counter);
                buff[0] = '\0';
                counter = 0;
                tokens[token_counter++] = token;
                return tokens;
            }
            case '(': {
                buff[counter++] = line[i];
                Token token;
                token.kind = TOK_PAREN_OPEN_DELIM;
                token.value = malloc(sizeof(char) * 1024);
                strncpy(token.value, buff, counter);
                buff[0] = '\0';
                counter = 0;
                tokens[token_counter++] = token;
                delim_stack[delim_tracker] = line[i];
                break;
            }
            case ')': {
                buff[counter++] = line[i];
                if (delim_stack[delim_tracker] != '(') {
                    fprintf(stderr, "[ERROR] Expected to close expression\n");
                    return NULL;
                }
                Token token;
                token.kind = TOK_PAREN_CLOSE_DELIM;
                token.value = malloc(sizeof(char) * 1024);
                strncpy(token.value, buff, counter);
                buff[0] = '\0';
                counter = 0;
                tokens[token_counter++] = token;
                delim_stack[--delim_tracker] = 0;
                break;
            }
            case '"': {
                if (is_string) {
                    Token token;
                    token.kind = TOK_STRING;
                    token.value = malloc(sizeof(char) * 1024);
                    strncpy(token.value, buff, counter);
                    buff[0] = '\0';
                    counter = 0;
                    tokens[token_counter++] = token;
                }
                is_string = !is_string;
                break;
            }
        }
    }

    return tokens;
}
