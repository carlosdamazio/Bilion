#include <bits/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "billion.h"

static void print_pinned_err(char *line, int pos)
{
    fprintf(stderr, "[ERROR] %s", line);
    fprintf(stderr, "[ERROR] %*s", pos, "");
    fprintf(stderr, "^\n");
}

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

static Token new_tok(char *buff, int kind, int counter, int lineno, int pos)
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
Token* lex(FileInfo *fi, char *line)
{
    Token *tokens = malloc(sizeof(Token) * LEX_CAP);
    
    /* Not using pointers due to new_tok returning a new variable from
     same address, poluting the delimiter stack with incorrect
     tokens.*/
    Token delim_stack[10];
    char *buff = initialize_buffer();
    bool is_string = false;
    
    int counter = 0;
    int delim_index = 0;
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
                Token token = new_tok(buff, TOK_PRINT, counter, fi->curr_line, i); 
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
            }
            continue;
        }
        
        // Delimiters check
        switch (line[i]) {
            case ';': {
                buff[counter++] = line[i];
                Token token = new_tok(buff, TOK_END_EXPR_DELIM, counter, 
                                      fi->curr_line, i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                
                if (delim_index != 0) {
                    print_pinned_err(line, delim_stack[delim_index-1].pos);
                    fprintf(stderr, "[ERROR] %s:%d:%d - Delimiter not closed\n",
                            fi->filename,
                            delim_stack[delim_index-1].lineno,
                            delim_stack[delim_index-1].pos+1);
                    free_tokens(tokens);
                    return NULL;
                }
                return tokens;
            }
            case '(': {
                buff[counter++] = line[i];
                Token token = new_tok(buff, TOK_PAREN_OPEN_DELIM, counter,
                                      fi->curr_line, i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                delim_stack[delim_index++] = token;
                break;
            }
            case ')': {
                buff[counter++] = line[i];
                if (strcmp(delim_stack[delim_index-1].value, "(") != 0) {
                    print_pinned_err(line, delim_stack[delim_index-1].pos);
                    fprintf(stderr, "[ERROR] %s:%d:%d - Expected to match "
                            "delimiters\n", fi->filename,
                            delim_stack[delim_index-1].lineno, 
                            delim_stack[delim_index-1].pos);
                    free_tokens(tokens);
                    return NULL;
                }
                Token token = new_tok(buff, TOK_PAREN_CLOSE_DELIM, counter, 
                                      fi->curr_line, i);
                reset_buffer(buff, &counter);
                tokens[token_counter++] = token;
                delim_stack[delim_index--] = empty_tok;
                break;
            }
            case '"': {
                if (is_string) {
                    Token token = new_tok(buff, TOK_STRING, counter,
                                          fi->curr_line, i);
                    reset_buffer(buff, &counter);
                    tokens[token_counter++] = token;
                }
                is_string = !is_string;
                break;
            }
        }
    }

    return tokens;
}

