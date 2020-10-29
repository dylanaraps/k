#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "str.h"

str *str_init(size_t l) {
    str *s = malloc(sizeof (*s) + l + 1);

    if (s) {
         s->cap = l + 1;
         s->len = 0;
        *s->buf = 0;
    }

    return s;
}

int str_alloc(str **s, size_t l) {
    str *s2 = realloc(*s, sizeof (*s2) + (*s)->cap + l);

    if (!s2) {
        return -1;
    }

    *s = s2;
    (*s)->cap += l;

    return 0;
}

int str_push_c(str **s, int c) {
    if ((*s)->len + 1 >= (*s)->cap) {
        if (str_alloc(s, ((*s)->cap + ((*s)->cap >> 1))) < 0) {
            return -1;
        }
    }

    (*s)->buf[(*s)->len] = c;
    (*s)->buf[(*s)->len += 1] = 0;

    return 0;
}

int str_push_l(str **s, const char *d, size_t l) {
    if (!d || l == 0) {
        return -1;
    }

    if (((*s)->len + l) >= (*s)->cap) {
        if (str_alloc(s, (l + (l >> 1))) < 0) {
            return -1;
        }
    }

    for (size_t i = 0; i < l; i++) {
        if (str_push_c(s, d[i]) < 0) {
            return -1;
        }
    }

    return 0;
}

int str_push_s(str **s, const char *d) {
    if (!d) {
        return -1;
    }

    size_t l = 0;

    while (d[l]) l++;

    return str_push_l(s, d, l);
}

int str_undo_l(str **s, size_t l) {
    if (l > (*s)->len) {
        return -1;
    }

    (*s)->buf[(*s)->len -= l] = 0;

    return 0;
}

int str_undo_s(str **s, const char *d) {
    if (!d) {
        return -1;
    }

    size_t l = 0;

    while (d[l]) l++;

    return str_undo_l(s, l);
}

int str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(NULL, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (((*s)->len + (size_t) l1) >= (*s)->cap) {
        if (str_alloc(s, (size_t) l1) < 0) {
            return -1;
        }
    }

    int l2 = vsnprintf((*s)->buf + (*s)->len, (size_t) l1 + 1, f, ap);

    if (l1 == l2) {
        (*s)->len += (size_t) l1;
        return 0;
    }

    return -1;
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

