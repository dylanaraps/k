#ifndef KISS_LOG_H_
#define KISS_LOG_H_

#include <stdio.h>

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

#endif
