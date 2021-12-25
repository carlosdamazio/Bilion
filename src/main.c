#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "billion.h"

int main (int argc, char *argv[])
{
    char line [256];
    int lineno = 0;
    
    if (argc < 2) {
        fprintf(stderr, "[ERROR] Provide enough arguments to compiler\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r+");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] File doesn't exist\n");
        return 1;
    }

    // Lexical analysis
    while(fgets(line, sizeof(line), file)) {
        Token *tokens = lex(line, ++lineno); 
        if (tokens == NULL) {
            fprintf(stderr, "[ERROR] Lexing error, exiting...\n");
            return 1;
        }
        
        Token *start = tokens;
        for (size_t i = 0; i < strlen(line); i++) {
            if (tokens->value == NULL)
                break;
            fprintf(stdout, "[DEBUG] Token = (type: %d, lineno: %d, pos: %d, "
                            "value: %s)\n", tokens->kind, tokens->lineno,
                                            tokens->pos, tokens->value);
            tokens++;
        }
        tokens = start;
        free_tokens(tokens);
    }

    fclose(file);
    return 0;
}
