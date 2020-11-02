#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

#define HDR_LEN (sizeof (size_t) * 2)
#define CAP_OFF 2
#define LEN_OFF 1

str *str_init(size_t l) {
    str *n = malloc(sizeof *n + l + 1);

    if (n) {
        n->len = 0;
        n->cap = l + 1;
    }

    return n;
}

int str_alloc(str **s, size_t l) {
    str *n = realloc(*s, sizeof *n + (*s)->cap + l);

    if (!n) {
        return -ENOMEM;
    }

    n->cap += l;
    *s = n;

    return 0;
}

int str_alloc_maybe(str **s, size_t l) {
    if ((*s)->len + l < (*s)->cap) {
        return 0;
    }

    return str_alloc(s, (l + (l >> 1)) + 1) < 0;
}

int str_push_l(str **s, const char *d, size_t l) {
    if (str_alloc_maybe(s, l) < 0) {
        return -ENOMEM;
    }

    memcpy((*s)->buf + (*s)->len, d, l + 1);
    str_set_len((*s), (*s)->len + l);

    return 0;
}

int str_push_s(str **s, const char *d) {
    if (!d || !*d) {
        return -EINVAL;
    }

    return str_push_l(s, d, strlen(d));
}

int str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(0, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (str_alloc_maybe(s, (size_t) l1) < 0) {
        return -ENOMEM;
    }

    if (vsnprintf((*s)->buf + (*s)->len, (size_t) l1 + 1, f, ap) != l1) {
        return -1;
    }

    str_set_len((*s), (*s)->len + (size_t) l1);
    return 0;
}

int str_printf(str **s, const char *f, ...) {
    va_list ap;
    va_start(ap, f);
    int ret = str_vprintf(s, f, ap);
    va_end(ap);
    return ret;
}

void str_undo_c(str **s, int d) {
    for (; (*s)->len && (*s)->buf[(*s)->len] == d; (*s)->len--);
    (*s)->buf[(*s)->len] = 0;
}

void str_free(str **s) {
    if (*s) {
        free(*s);
        *s = NULL;
    }
}

