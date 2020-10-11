#ifndef KISS_STR_H_
#define KISS_STR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct str {
    size_t len;
    size_t cap;
    char *buf;
} str;

#define str_init(s)                        \
    do {                                   \
        if (!(*s)) {                       \
            (*s) = calloc(1, sizeof(str)); \
            if (!(*s)) {                   \
                perror("calloc");          \
                exit(1);                   \
            }                              \
        }                                  \
    } while (0)

#define str_free(s)          \
    do {                     \
        if ((*s)) {          \
            free((*s)->buf); \
            free((*s));      \
        }                    \
    } while (0)

#define str_undo(s, d)                         \
    do {                                       \
        (*s)->buf[(*s)->len -= strlen(d)] = 0; \
    } while (0)

#define str_alloc(s, l)                                         \
    do {                                                        \
        str_init(s);                                            \
        if (((*s)->len + l) >= (*s)->cap) {                     \
            (*s)->buf = realloc((*s)->buf, (*s)->cap += l * 2); \
            if (!(*s)->buf) {                                   \
                perror("realloc");                              \
                exit(1);                                        \
            }                                                   \
        }                                                       \
    } while (0)

#define str_push(s, d)                                \
    do {                                              \
        if (d && d[0]) {                              \
            size_t _l = strlen(d);                    \
            str_alloc(s, _l + 1);                     \
            memcpy((*s)->buf + (*s)->len, d, _l + 1); \
            (*s)->buf[(*s)->len += _l] = 0;           \
        }                                             \
    } while (0)

#define str_fmt(s, f, ...)                                  \
    do {                                                    \
        int _l2 = snprintf(NULL, 0, f, __VA_ARGS__);        \
        if (_l2 > 0) {                                      \
            str_alloc(s, (size_t) _l2 + 1);                 \
            if (snprintf((*s)->buf + (*s)->len,             \
                (size_t) _l2 + 1, f, __VA_ARGS__) != _l2) { \
                perror("snprintf");                         \
                exit(1);                                    \
            }                                               \
            (*s)->len += (size_t) _l2;                      \
        }                                                   \
    } while (0)

#define str_path(s)                                        \
    do {                                                   \
        size_t _l3 = 1;                                    \
        for (; (*s)->buf[(*s)->len - _l3] == '/'; _l3++) { \
            (*s)->buf[(*s)->len - _l3] = 0;                \
        }                                                  \
        (*s)->len -= _l3 - 1;                              \
    } while (0)

#define str_getline(s, f)                             \
    do {                                              \
        str_zero(s);                                  \
        int _l4 = getline(&(*s)->buf, &(*s)->cap, f); \
        if (_l4 != -1) {                              \
            (*s)->len = (size_t) _l4;                 \
            if ((*s)->buf[(*s)->len - 1] == '\n') {   \
                (*s)->buf[(*s)->len - 1] = 0;         \
            }                                         \
        }                                             \
    } while (0)

#define str_zero(s)                          \
    do {                                     \
        if ((*s)) {                          \
            memset((*s)->buf, 0, (*s)->len); \
            (*s)->len = 0;                   \
        }                                    \
    } while (0)

#endif
