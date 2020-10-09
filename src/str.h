#ifndef KISS_STR_H_
#define KISS_STR_H_

#include <stdio.h>
#include <sys/types.h>

typedef struct str {
    size_t len;
    size_t cap;
    char *buf;
} str;

void str_alloc(str *, size_t);
void str_push(str *, const char *);
void str_undo(str *, const char *);
void str_free(str *);

#define str_from(s, f, t) \
    do {                                       \
        int l = snprintf(NULL, 0, f, t);       \
        if (l < 1) return;                     \
        str_alloc(s, (size_t) l);              \
        int e = snprintf(s->buf + s->len,      \
                        (size_t) l + 1, f, t); \
        if (e != l) s->buf[s->len] = 0;        \
    } while (0)

#endif
