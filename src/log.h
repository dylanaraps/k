#ifndef KISS_LOG_H_
#define KISS_LOG_H_

#include <stdio.h>

#include "str.h"

#define msg(...) do {             \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr);          \
} while (0)

#define die(...) do {             \
    msg(__VA_ARGS__);             \
    exit(EXIT_FAILURE);           \
} while (0)

#define die_free_str(s, ...) do { \
    str_free(s);                  \
    die(__VA_ARGS__);             \
} while (0)

#endif
