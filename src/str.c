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
#include "str.h"

str *str_init(size_t l) {
    str *n = malloc(sizeof *n + l + 1);

    if (n) {
        n->len = 0;
        n->cap = l + 1;
    }

    return n;
}

static int str_alloc(str **s, size_t l) {
    str *n = realloc(*s, sizeof *n + (*s)->cap + l);

    if (!n) {
        return -ENOMEM;
    }

    n->cap += l;
    *s = n;

    return 0;
}

static int str_alloc_maybe(str **s, size_t l) {
    if ((*s)->len + l < (*s)->cap) {
        return 0;
    }

    return str_alloc(s, (l + (l >> 1)) + 1);
}

int str_push_l(str **s, const char *d, size_t l) {
    if (str_alloc_maybe(s, l) < 0) {
        return -ENOMEM;
    }

    memcpy((*s)->buf + (*s)->len, d, l + 1);
    str_set_len(*s, (*s)->len + l);

    return 0;
}

int str_push_s(str **s, const char *d) {
    if (!d || !*d) {
        return -EINVAL;
    }

    return str_push_l(s, d, strlen(d));
}

int str_push_c(str **s, int d) {
    if (str_alloc_maybe(s, 1) < 0) {
        return -ENOMEM;
    }

    (*s)->buf[(*s)->len++] = d;
    str_set_len(*s, (*s)->len);

    return 0;
}

int str_undo_c(str **s, int d) {
    if ((*s)->buf[(*s)->len - 1] == d) {
        str_set_len(*s, (*s)->len - 1);
        return 0;
    }

    return -1;
}

int str_rstrip(str **s, int d) {
    int n = 0;

    while ((*s)->buf[(*s)->len - 1] == d) {
        str_set_len(*s, (*s)->len - 1);
        n++;
    }

    return n;
}

int str_getline(str **s, FILE *f, size_t l) {
    do {
        if (ferror(f) || feof(f)) {
            return -1;
        }

        if (str_alloc_maybe(s, l) < 0) {
            return -ENOMEM;
        }

        if (!fgets((*s)->buf + (*s)->len, l, f)) {
            return -1;
        }

        (*s)->len += strnlen((*s)->buf + (*s)->len, l);

    } while ((*s)->buf[(*s)->len - 1] != '\n');

    str_set_len(*s, (*s)->len - 1);
    return 0;
}

static int str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(NULL, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (str_alloc_maybe(s, (size_t) l1) < 0) {
        return -1;
    }

    if (vsnprintf((*s)->buf + (*s)->len, (size_t) l1 + 1, f, ap) != l1) {
        return -1;
    }

    str_set_len(*s, (*s)->len + (size_t) l1);
    return 0;
}

int str_printf(str **s, const char *f, ...) {
    va_list ap;
    va_start(ap, f);
    int ret = str_vprintf(s, f, ap);
    va_end(ap);
    return ret;
}

void str_free(str **s) {
    if (*s) {
        free(*s);
        *s = NULL;
    }
}

