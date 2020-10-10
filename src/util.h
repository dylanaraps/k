#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <stdio.h>
#include <sys/types.h>

char *path_basename(char *);
int PATH_prepend(const char *, const char *);
int is_dir(const char *);
FILE *fopenat(const char *, const char *, const char *);
int mkdir_p(char *, const mode_t);
int mkdir_e(char *, const mode_t);

#define msg(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr); \
    } while (0)

#define die(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr); \
        exit(EXIT_FAILURE); \
    } while (0)

#endif
