#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <sys/types.h>

int mkdir_p(char *, const mode_t);
int rm_rf(const char *);
int is_dir(const char *);

#define msg(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr);          \
    } while (0)

#define die(...)            \
    do {                    \
        msg(__VA_ARGS__);   \
        exit(EXIT_FAILURE); \
    } while (0)

#define free_die(v, ...)  \
    do {                  \
        free(v);          \
        die(__VA_ARGS__); \
    } while (0)

#define str_free_die(s, ...)  \
    do {                      \
        str_free(s);          \
        die(__VA_ARGS__);     \
    } while (0)

#endif
