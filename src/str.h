#ifndef KISS_STR_H
#define KISS_STR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef char str;

str *str_alloc(str **s, size_t l);
int str_push_l(str **s, const char *d, size_t l);
int str_push_s(str **s, const char *d);
int str_undo_l(str **s, size_t l);
int str_undo_s(str **s, const char *d);
int str_vprintf(str **s, const char *f, va_list ap);
int str_printf(str **s, const char *f, ...);
void str_free(str **s);

#define str_init(l) str_alloc(NULL, l)
#define str_get_cap(s) ((s) ? ((size_t *)(s))[-2] : (size_t) 0)
#define str_get_len(s) ((s) ? ((size_t *)(s))[-1] : (size_t) 0)

#define str_set_cap(s, l) do {      \
    if (*s) {                       \
        ((size_t *)(*s))[-2] = (l); \
    }                               \
} while(0) 

#define str_set_len(s, l) do {      \
    if (*s) {                       \
        ((size_t *)(*s))[-1] = (l); \
        (*s)[(l)] = (size_t) 0;     \
    }                               \
} while(0) 

#endif
