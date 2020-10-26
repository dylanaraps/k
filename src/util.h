#ifndef KISS_UTIL_H
#define KISS_UTIL_H

#include <stdio.h>
#include <stdlib.h>

const char *xgetenv(const char *var, const char *fallback);
char *path_normalize(char *d);
void mkdir_die(const char *d);
int mkdir_p(const char* d);
int run_cmd(const char *cmd);

#define msg(...) do {             \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr);          \
} while (0)

#define die(...) do {                           \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
    exit(EXIT_FAILURE);                         \
} while (0)

#endif
