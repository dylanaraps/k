#ifndef KISS_STR_H
#define KISS_STR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct str {
    size_t len;
    size_t cap;
    char buf[];
} str;

str *str_init(size_t l);
int str_alloc(str **s, size_t l);
int str_push_c(str **s, int c);
int str_push_l(str **s, const char *d, size_t l);
int str_push_s(str **s, const char *d);
int str_undo_l(str **s, size_t l);
int str_undo_s(str **s, const char *d);
int str_vprintf(str **s, const char *f, va_list ap);
int str_printf(str **s, const char *f, ...);
void str_free(str **s);

#endif
