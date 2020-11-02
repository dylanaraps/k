#ifndef KISS_STR_H
#define KISS_STR_H

#include <stdarg.h>
#include <stdlib.h>

typedef struct {
    size_t cap;
    size_t len;
    char buf[];
} str;

#define str_set_len(s, l) ((s->buf)[(s->len) = (l)] = 0)

str *str_init(size_t l);
int str_alloc(str **s, size_t l);
int str_alloc_maybe(str **s, size_t l);
int str_push_l(str **s, const char *d, size_t l);
int str_push_s(str **s, const char *d);
int str_vprintf(str **s, const char *f, va_list ap);
int str_printf(str **s, const char *f, ...);
void str_undo_c(str **s, int d);
void str_free(str **s);

#endif
