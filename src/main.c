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
        LexResult *result = lex(file_info, line); 
        if (result->stack_trace != NULL) {
            fprintf(stderr, "%s", result->stack_trace);
            fprintf(stderr, "[ERROR] Lexing error, exiting...\n");
            free_file_info(file_info);
            free_lex_result(result);
            fclose(file);
            return 1;
        }
        
        Token *start = result->tokens;
        for (size_t i = 0; i < strlen(line); i++) {
            if (result->tokens->value == NULL)
                break;
            fprintf(stdout, "[DEBUG] Token = (type: %d, lineno: %d, pos: %d, "
                            "value: %s)\n",
                            result->tokens->kind,
                            result->tokens->lineno,
                            result->tokens->pos,
                            result->tokens->value);
            result->tokens++;
        }
        result->tokens = start;
        free(result->stack_trace);
    }

    free_file_info(file_info);
    fclose(file);
    return 0;
}
