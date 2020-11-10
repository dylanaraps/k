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

#define HDR_SIZE (sizeof (size_t) * 2)

#define str_raw(s) ((s) ? (*s) ? ((size_t *) (*s) - 2) : 0 : 0)

str *str_alloc(str **s, size_t l) {
    size_t *n = realloc(str_raw(s), HDR_SIZE + l);

    if (!n) {
        return NULL;
    }

    n[0] = l;
    n[1] = 0;

    return (char *) &n[2];
}

static int str_alloc_maybe(str **s, size_t l) {
    size_t sl = str_len(*s);

    if ((sl + l) > str_cap(*s)) {
        str *n = str_alloc(s, str_cap(*s) + (l + (l >> 1)));

        if (!n) {
            return -ENOMEM;
        }

        str_set_len(n, sl);
        *s = n;
    }

    return 0;
}

int str_push_l(str **s, const char *d, size_t l) {
    if (str_alloc_maybe(s, l) < 0) {
        return -ENOMEM;
    }

    memcpy(*s + str_len(*s), d, l + 1);
    str_set_len(*s, str_len(*s) + l);

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

    (*s)[str_len(*s)] = d;
    str_set_len(*s, str_len(*s) + 1);

    return 0;
}

int str_undo_c(str **s, int d) {
    if ((*s)[str_len(*s) - 1] == d) {
        str_set_len(*s, str_len(*s) - 1);
        return 0;
    }

    return -1;
}

void str_rstrip(str **s, int d) {
    while (str_undo_c(s, d) == 0);
}

int str_getline(str **s, FILE *f, size_t l) {
    do {
        if (ferror(f) || feof(f)) {
            return -1;
        }

        if (str_alloc_maybe(s, l) < 0) {
            return -ENOMEM;
        }

        if (!fgets(*s + str_len(*s), l, f)) {
            return -1;
        }

        str_set_len(*s, str_len(*s) + strnlen(*s + str_len(*s), l));

    } while (*(*s + str_len(*s) - 1) != '\n');

    str_set_len(*s, str_len(*s) - 1);
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

    if (vsnprintf(*s + str_len(*s), (size_t) l1 + 1, f, ap) != l1) {
        return -1;
    }

    str_set_len(*s, str_len(*s) + (size_t) l1);
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
        free((size_t *) *s - 2);
        *s = NULL;
    }
}

