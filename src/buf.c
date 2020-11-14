/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "buf.h"

buf *buf_alloc(buf **s, size_t l) {
    size_t *p = buf_raw(s);
    size_t *n = realloc(p, (sizeof (size_t) * 2) + l);

    if (!n) {
        return NULL;
    }

    n[0] = l;
    n[1] = p ? n[1] : 0;

    return (char *) &n[2];
}

static int buf_alloc_maybe(buf **s, size_t l) {
    if ((buf_len(*s) + l) <= buf_cap(*s)) {
        return 0;
    }

    buf *n = buf_alloc(s, buf_cap(*s) + (l + (l >> 1)));

    if (!n) {
        return -ENOMEM;
    }

    *s = n;
    return 0;
}

int buf_push_l(buf **s, const char *d, size_t l) {
    if (buf_alloc_maybe(s, l) < 0) {
        return -ENOMEM;
    }

    memcpy(*s + buf_len(*s), d, l + 1);
    buf_set_len(*s, buf_len(*s) + l);
    return 0;
}

int buf_push_s(buf **s, const char *d) {
    if (!d || !*d) {
        return -EINVAL;
    }

    return buf_push_l(s, d, strlen(d));
}

int buf_push_c(buf **s, int d) {
    if (buf_alloc_maybe(s, 1) < 0) {
        return -ENOMEM;
    }

    (*s)[buf_len(*s)] = d;
    buf_set_len(*s, buf_len(*s) + 1);
    return 0;
}

int buf_undo_c(buf **s, int d) {
    if (buf_end(*s) == d) {
        buf_set_len(*s, buf_len(*s) - 1);
        return 0;
    }

    return -1;
}

void buf_rstrip(buf **s, int d) {
    while (buf_undo_c(s, d) == 0);
}

int buf_set(buf **s, int c, size_t l) {
    if (buf_alloc_maybe(s, l + 1) < 0) {
        return -ENOMEM;
    }

    memset(*s + buf_len(*s), c, l);
    buf_set_len(*s, buf_len(*s) + l);

    return 0;
}

int buf_getline(buf **s, FILE *f, size_t l) {
    do {
        if (ferror(f) || feof(f)) {
            return -1;
        }

        if (buf_alloc_maybe(s, l) < 0) {
            return -ENOMEM;
        }

        if (!fgets(*s + buf_len(*s), l, f)) {
            return -1;
        }

        buf_set_len(*s, buf_len(*s) + strnlen(*s + buf_len(*s), l));

    } while (buf_end(*s) != '\n');

    buf_set_len(*s, buf_len(*s) - 1);
    return 0;
}

static int buf_vprintf(buf **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(NULL, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (buf_alloc_maybe(s, (size_t) l1) < 0) {
        return -ENOMEM;
    }

    if (vsnprintf(*s + buf_len(*s), (size_t) l1 + 1, f, ap) != l1) {
        return -1;
    }

    buf_set_len(*s, buf_len(*s) + (size_t) l1);
    return 0;
}

int buf_printf(buf **s, const char *f, ...) {
    va_list ap;
    va_start(ap, f);
    int ret = buf_vprintf(s, f, ap);
    va_end(ap);
    return ret;
}

size_t buf_scan(buf **s, size_t l, int c) {
    size_t i = l;

    for (; i < buf_len(*s); i++) {
        if ((*s)[i] == c) {
            (*s)[i] = 0;
            i++;
            break;
        }
    }

    return i;
}

void buf_free(buf **s) {
    free(buf_raw(s));
}

