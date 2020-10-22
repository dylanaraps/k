#ifndef KISS_STR_H_
#define KISS_STR_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct str {
    size_t len;
    size_t cap;
    size_t err;
    char buf[];
} str;

enum str_error {
    STR_OK,
    STR_ERROR,
    STR_ENOMEM,
    STR_EINVAL,
    STR_EOF,
};

str *str_init(size_t l);
str *str_init_die(size_t l);
void str_alloc(str **s, size_t l);
void str_push_c(str **s, int c);
void str_push_l(str **s, const char *d, size_t l);
void str_push_s(str **s, const char *d);
void str_undo_l(str **s, size_t l);
void str_undo_s(str **s, const char *d);
str *str_dup(str **s);
void str_getline(str **s, FILE *f);
void str_vprintf(str **s, const char *f, va_list ap);
void str_printf(str **s, const char *f, ...);
size_t str_tok(str *s, char c, size_t tok);
void str_path_normalize(str **s);
size_t str_rchr(str *s, int c);
void str_free(str *s);

#endif
