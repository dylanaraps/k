#ifndef KISS_STR_H
#define KISS_STR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef char str;

size_t str_get_cap(str **s);
size_t str_get_len(str **s);
void str_set_cap(str **s, size_t l);
void str_set_len(str **s, size_t l);

str *str_init(size_t l);
str *str_alloc(str **s, size_t l);
int str_maybe_alloc(str **s, size_t l);
int str_push_l(str **s, const char *d, size_t l);
int str_push_s(str **s, const char *d);
int str_undo_l(str **s, size_t l);
int str_undo_s(str **s, const char *d);
int str_vprintf(str **s, const char *f, va_list ap);
int str_printf(str **s, const char *f, ...);
void str_free(str **s);

#endif
