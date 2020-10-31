#ifndef KISS_UTIL_H
#define KISS_UTIL_H

#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int mkdirat_p(int fd, const char* d);
int mkdir_p(const char* d);
int run_cmd(const char *cmd);
int is_dir(const char *path);
int file_print_line(FILE *f);
int mkopenat(int fd, const char *path);
FILE *fopenat(int fd, const char *path, int o, const char *m, int p);
ssize_t getline_kiss(char **line, char **f1, char **f2, size_t *size, FILE *f);
char *bname(char *s);
char *path_fix(char *p);

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

#define err_no(...) do {                        \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s", strerror(errno));   \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

#endif
