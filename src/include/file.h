#include <stdio.h>

typedef struct {
    const char *filename;
    const char *encoding;
    int        curr_line;
} FileInfo;

void      free_file_info(FileInfo *fi);
FileInfo* new_file_info(FILE *fp, int curr_line);
