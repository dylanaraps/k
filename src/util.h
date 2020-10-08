#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <stdio.h>

char *xgetcwd(void);
int PATH_prepend(const char *path, const char *var);
int is_dir(const char *);
FILE *fopenat(const char *d, const char *f, const char *m);

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
