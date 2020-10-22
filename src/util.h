#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

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
