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

    FileInfo *file_info = new_file_info(file, lineno); 

    // Lexical analysis
    while(fgets(line, sizeof(line), file)) {
        file_info->curr_line++;
        Lexer *lexer = lex(file_info, line);
        if (strcmp(lexer->stack_trace, STACK_TRACE_HEADER) != 0) {
            fprintf(stderr, "%s", lexer->stack_trace);
            fprintf(stderr, "[ERROR] Lexing error, exiting...\n");
            free_file_info(file_info);
            free_lexer(lexer);
            fclose(file);
            return 1;
        }
        
        for (size_t i = 0; i < strlen(line); i++) {
            if (lexer->tokens[i].value == NULL)
                break;
            fprintf(stdout, "[DEBUG] Token = (type: %d, lineno: %d, pos: %d, "
                            "value: %s)\n",
                            lexer->tokens[i].kind,
                            lexer->tokens[i].lineno,
                            lexer->tokens[i].pos,
                            lexer->tokens[i].value);
        }
        free_lexer(lexer);
    }

    free_file_info(file_info);
    fclose(file);
    return 0;
}
