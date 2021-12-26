#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "billion.h"

#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 1
#endif

static char* get_filename(FILE *fp)
{
    int fd;
    char fd_path[255];
    char *filename = malloc(sizeof(char) * 255);
    ssize_t n;

    fd = fileno(fp);
    sprintf(fd_path, "/proc/self/fd/%d", fd);
    n = readlink(fd_path, filename, 255);
    if (n < 0)
        return NULL;
    filename[n] = '\0';
    return filename;
}

void free_file_info(FileInfo *fi)
{
    free((void*)fi->filename);
    free(fi);
}

FileInfo* new_file_info(FILE *fp, int curr_line)
{
    FileInfo *fi = malloc(sizeof(FileInfo));
    fi->filename = get_filename(fp);
    fi->curr_line = curr_line;

    return fi;
}
