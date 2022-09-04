#include <bits/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "billion.h"

static void
print_pinned_err(char *line, int pos)
{
    fprintf(stderr, "[ERROR] %s", line);
    fprintf(stderr, "[ERROR] %*s", pos, "");
    fprintf(stderr, "^\n");
}

static void
reset_buffer(Lexer *lexer, int *counter)
{
    for (int i = 0; i < lexer->buffer_sz; i++) {
        lexer->buffer[i] = ' ';
    }
    *counter = 0;
    lexer->buffer[*counter] = '\0';
}

static Token
new_tok(char *buff, int kind, int lineno, int pos)
{
    Token token;
    token.kind = kind;
    token.lineno = lineno;
    token.pos = pos;
    token.value = malloc(sizeof(char) * LEX_CAP);
    for (int i = 0; i < LEX_CAP; i++) {
        token.value[i] = ' ';
    }
    token.value[0] = '\0';
    strcpy(token.value, buff);
    return token;
}

static Lexer*
new_lexer(void) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->buffer = malloc(sizeof(char)*LEX_CAP);
    lexer->buffer_sz = LEX_CAP;
    for (int i = 0; i < LEX_CAP; i++) {
        lexer->buffer[i] = ' ';
    }
    lexer->buffer[0] = '\0';

    lexer->stack_trace = malloc(sizeof(char)*LEX_CAP);;
    lexer->stack_trace_sz = LEX_CAP;
    for (int i = 0; i < LEX_CAP; i++) {
        lexer->stack_trace[i] = ' ';
    }
    lexer->stack_trace[0] = '\0';

    lexer->tokens = malloc(sizeof(Token)*LEX_CAP);
    for (int i = 0; i < LEX_CAP; i++) {
        lexer->tokens[i] = empty_tok;
    }

    return lexer;
}

void
free_tokens(Token *tokens)
{
    for (int i = 0; i < LEX_CAP; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}

void
free_lexer(Lexer *lexer)
{
    free_tokens(lexer->tokens);
    free(lexer->buffer);
    free(lexer->stack_trace);
    free(lexer);
}

void
lex_error(Lexer *lexer, const char *msg, char *line, FileInfo *fi, int counter, int delim_index) {
    print_pinned_err(line, lexer->delim_stack[delim_index-1].pos);
    sprintf(lexer->buffer,
            "[ERROR] %s:%d:%d - %s\n",
            fi->filename,
            lexer->delim_stack[delim_index-1].lineno,
            lexer->delim_stack[delim_index-1].pos+1,
            msg);
    strcat(lexer->stack_trace, lexer->buffer);
    reset_buffer(lexer, &counter);
}

// This function lexes a line into an array of tokens, with type and value.
Lexer*
lex(FileInfo *fi, char *line)
{
    bool is_string = false;
    int counter, delim_index, token_counter = 0;

    Lexer *lexer = new_lexer();
    if (lexer == NULL) {
        fprintf(stderr, "[ERROR] couldn't create lexer\n");
        return NULL;
    }
    
    // Initialize stack trace header
    strcat(lexer->stack_trace, STACK_TRACE_HEADER);

    for (size_t i = 0; i < strlen(line); i++) {
        // String literal check
        if (is_string && line[i] != '"') {
            lexer->buffer[counter++] = line[i];
            lexer->buffer[counter] = '\0';
            continue;
        }
        
        // Ignore spaces
        if (isspace(line[i]))
            continue;
        
        // Keyword check
        if (isalpha(line[i])) {
            lexer->buffer[counter++] = line[i];
            lexer->buffer[counter] = '\0';
            // see if identifier is a keyword, and reset it afterwards
            if (strcmp(lexer->buffer, "exposed") == 0) {
                Token token = new_tok(lexer->buffer, TOK_PRINT, counter, fi->curr_line);
                reset_buffer(lexer, &counter);
                lexer->tokens[token_counter++] = token;
            }
            continue;
        }
        
        // Delimiters check
        switch (line[i]) {
            case ';': {
                lexer->buffer[counter++] = line[i];
                lexer->buffer[counter] = '\0';
                Token token = new_tok(lexer->buffer, TOK_END_EXPR_DELIM, counter,fi->curr_line);
                reset_buffer(lexer, &counter);
                lexer->tokens[token_counter++] = token;

                if (delim_index != 0) {
                    lex_error(lexer, "delimiter not closed", line, fi, counter, delim_index);
                }

                return lexer;
            }
            case '(': {
                lexer->buffer[counter++] = line[i];
                lexer->buffer[counter] = '\0';
                Token token = new_tok(lexer->buffer, TOK_PAREN_OPEN_DELIM, counter,fi->curr_line);
                lexer->tokens[token_counter++] = token;
                lexer->delim_stack[delim_index++] = token;
                break;
            }
            case ')': {
                lexer->buffer[counter++] = line[i];
                lexer->buffer[counter] = '\0';
                if (delim_index == 0 || strcmp(lexer->delim_stack[delim_index-1].value, "(") != 0) {
                    lex_error(lexer, "expected to match delimiters\n", line, fi, counter, delim_index);
                    continue;
                }
                Token token = new_tok(lexer->buffer, TOK_PAREN_CLOSE_DELIM, counter,fi->curr_line);
                lexer->tokens[token_counter++] = token;
                lexer->delim_stack[delim_index--] = empty_tok;
                break;
            }
            case '"': {
                if (is_string) {
                    Token token = new_tok(lexer->buffer, TOK_STRING, counter, fi->curr_line);
                    lexer->tokens[token_counter++] = token;
                }
                is_string = !is_string;
                break;
            }
            default: {
                Token token = new_tok(lexer->buffer, TOK_INVALID, counter, fi->curr_line);
                lexer->tokens[token_counter++] = token;
                lex_error(lexer, "unknown token", line, fi, counter, delim_index);
            }
        }
        reset_buffer(lexer, &counter);
    }

    if (delim_index != 0) {
        lex_error(lexer, "delimiter not closed", line, fi, counter, delim_index);
    }

    if (lexer->tokens[token_counter-1].kind != TOK_END_EXPR_DELIM) {
        lex_error(lexer, "no end of expr delimiter", line, fi, counter, delim_index);
    }

    return lexer;
}

