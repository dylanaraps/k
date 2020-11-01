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
    return str_alloc(0, l);
}

str *str_alloc(str **s, size_t l) {
    size_t *n = realloc(s ? *s ? (size_t *) *s - CAP_OFF : 0 : 0,
        HDR_LEN + str_get_cap(s) + l + 1);

    if (!n) {
        return 0;
    }

    n[0] = (s ? *s ? n[0] : 0 : 0) + l + 1;
    n[1] = (s ? *s ? n[1] : 0 : 0);

    return (char *) &n[CAP_OFF];
}

int str_maybe_alloc(str **s, size_t l) {
    if (str_get_len(s) + l < str_get_cap(s)) {
        return 0;
    }

    str *n = str_alloc(s, (l + (l >> 1)));

    if (!n) {
        return -ENOMEM;
    }

    *s = n;
    return 0;
}

int str_push_l(str **s, const char *d, size_t l) {
    if (str_maybe_alloc(s, l) < 0) {
        return -ENOMEM;
    }

    memcpy(*s + str_get_len(s), d, l + 1);
    str_set_len(s, str_get_len(s) + l);

    return 0;
}

int str_push_s(str **s, const char *d) {
    return d ? str_push_l(s, d, strlen(d)) : -EINVAL;
}

int str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(0, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (str_maybe_alloc(s, (size_t) l1) < 0) {
        return -ENOMEM;
    }

    if (vsnprintf(*s + str_get_len(s), (size_t) l1 + 1, f, ap) != l1) {
        return -1;
    }

    str_set_len(s, str_get_len(s) + (size_t) l1);
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
    size_t l = str_get_len(s) - 1;
    for (; l && *(*s + l--) == d ;);
    str_set_len(s, l);
}

size_t str_get_cap(str **s) {
    return s ? *s ? ((size_t *) *s)[-CAP_OFF] : 0 : 0;
}

size_t str_get_len(str **s) {
    return s ? *s ? ((size_t *) *s)[-LEN_OFF] : 0 : 0;
}

void str_set_cap(str **s, size_t l) {
    ((size_t *) *s)[-CAP_OFF] = l;
}

void str_set_len(str **s, size_t l) {
    ((size_t *) *s)[-LEN_OFF] = l;
    str_set_pos(s, l, 0);
}

void str_set_pos(str **s, size_t l, int c) {
    *(*s + l) = c;
}

void str_free(str **s) {
    if (*s) {
        free((size_t *) *s - CAP_OFF);
        *s = NULL;
    }
}

