#include <bits/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static char* initialize_buffer()
{
    static char buff[LEX_CAP];
    buff[LEX_CAP-1] = '\0';
    return buff;
}

static void reset_buffer(char buff[], int *counter)
{
    buff[0] = '\0';
    *counter = 0;
}

static Token new_tok(int kind, char *buff, int counter, int lineno, int pos)
{
    Token token;
    token.kind = kind;
    token.lineno = lineno;
    token.pos = pos;
    token.value = malloc(sizeof(char) * LEX_CAP);
    strncpy(token.value, buff, counter);
    return token;
}

void free_tokens(Token *tokens)
{
    Token *start = tokens;

    while (tokens->value != NULL)
        free(tokens++->value);

    tokens = start;
    free(tokens);
}

// This function lexes a line into an array of tokens, with type and value.
Token* lex(char *line, int lineno)
{
    Token *tokens = malloc(sizeof(Token) * LEX_CAP);
    Token *delim_stack = malloc(sizeof(Token) * 10);
    Token *delim_stack_start = delim_stack;
    char *buff = initialize_buffer();
    bool is_string = false;
    
    int counter = 0;
    int token_counter = 0;

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
                Token token = new_tok(TOK_PRINT, buff, counter, lineno, i); 
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
            }
            continue;
        }
        
        // Delimiters check
        switch (line[i]) {
            case ';': {
                buff[counter++] = line[i];
                Token token = new_tok(TOK_END_EXPR_DELIM, buff, counter, lineno,
                                      i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                
                if (delim_stack != NULL) {
                    fprintf(stderr, "[ERROR] line:%d:%d -  Unmatching delimiter\n",
                            lineno, delim_stack->pos);
                    free_tokens(tokens);
                    free(delim_stack_start);
                    return NULL;
                }
                return tokens;
            }
            case '(': {
                buff[counter++] = line[i];
                Token token = new_tok(TOK_PAREN_OPEN_DELIM, buff, counter,
                                      lineno, i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                delim_stack = &token;
                delim_stack++;
                break;
            }
            case ')': {
                buff[counter++] = line[i];
                if (strcmp(delim_stack->value,"(") != 0) {
                    fprintf(stderr, "[ERROR] line:%d:%ld - Expected to match delimiters\n", lineno, i);
                    free_tokens(tokens);
                    free(delim_stack_start);
                    return NULL;
                }
                Token token = new_tok(TOK_PAREN_CLOSE_DELIM, buff, counter, 
                                      lineno, i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                delim_stack--;
                delim_stack = NULL;
                break;
            }
            case '"': {
                if (is_string) {
                    Token token = new_tok(TOK_STRING, buff, counter, lineno, i);
                    reset_buffer(buff, &counter);
                    tokens[token_counter++] = token;
                }
                is_string = !is_string;
                break;
            }
        }
    }

    free(delim_stack_start);
    return tokens;
}

