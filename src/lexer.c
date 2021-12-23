#include <bits/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"


// This function resets char arrays.
void clear_array(char* arr)
{
    for (int i = 0; i < 256; i++) {
        arr[i] = 0;
    }
}

// This function lexes a line into an array of tokens, with type and value.
Token* lex(char *line)
{
    Token *tokens = malloc(sizeof(Token) * 256);
    bool is_string = false;
    char delim_stack[10];
    char buff[256];
    int counter = 0;
    int token_counter = 0;
    int delim_tracker = 0;

    if (tokens == NULL) {
        fprintf(stderr, "[ERROR] Couldn't allocate memory for token array");
        return NULL;
    }

    for (size_t i = 0; i < strlen(line); i++) {
        if (isspace(line[i]))
            continue;
        
        // String literal check
        if (is_string && line[i] != '"') {
            buff[counter++] = line[i];
            continue;
        }

        // Keyword check
        if (isalpha(line[i])) {
            buff[counter++] = line[i];
            // see if identifier is a keyword, and reset it afterwards
            if (strcmp(buff, "exposed") == 0) {
                Token token = {TOK_PRINT, "exposed"};
                tokens[token_counter++] = token;
                clear_array(buff);
                counter = 0;
            }
            continue;
        }
        
        // Delimiters check

        switch (line[i]) {
            case ';': {
                Token token = {TOK_END_EXPR_DELIM, ";"};
                tokens[token_counter++] = token;
                return tokens;
            }
            case '(': {
                Token token = {TOK_PAREN_OPEN_DELIM, "("};
                tokens[token_counter++] = token;
                delim_stack[delim_tracker] = line[i];
                break;
            }
            case ')': {
                if (delim_stack[delim_tracker] != '(') {
                    fprintf(stderr, "[ERROR] Expected to close expression\n");
                    return NULL;
                }
                Token token = {TOK_PAREN_CLOSE_DELIM, ")"};
                tokens[token_counter++] = token;
                delim_stack[--delim_tracker] = 0;
                break;
            }
            case '"': {
                if (is_string) {
                    char *value = malloc(sizeof(char) * counter);
                    strncpy(value, buff, counter);
                    Token token = {TOK_STRING, value};
                    tokens[token_counter++] = token;
                    clear_array(buff);
                    free(value);
                    counter = 0;
                }
                is_string = !is_string;
                break;
            }
        }
    }

    return tokens;
}
