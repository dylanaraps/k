#ifndef KISS_UTIL_H
#define KISS_UTIL_H

#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int mkdir_p(const char* d);
int run_cmd(const char *cmd);
int file_print_line(FILE *f);
int is_dir(const char *path);
int globat(const char *pwd, const char *query, int opt, glob_t *res);

#define msg(...) do {             \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr);          \
} while (0)

#define err(...) do {                           \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

#endif
