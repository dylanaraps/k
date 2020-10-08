#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <stdio.h>

size_t xgetcwd(char *p[]);
char *path_basename(char *, size_t);
int PATH_prepend(const char *, const char *);
int is_dir(const char *);
FILE *fopenat(const char *, const char *, const char *);

#define msg(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        puts(""); \
    } while (0)

#define die(...) \
    do { \
        msg(__VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } while (0)

#endif
