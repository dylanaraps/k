#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

str *str_alloc(str **s, size_t l) {
    size_t *s2 = realloc(*s ? (size_t *) *s - 2 : (size_t *) *s,
        (sizeof (size_t) * 2) + str_get_cap(*s) + l + 1);

    if (!s2) {
        return NULL;
    }

    s2[0] = (*s ? s2[0] : (size_t) 0) + l + 1; // cap
    s2[1] = (*s ? s2[1] : (size_t) 0);         // len

    return (char *) &s2[2];
}

int str_push_l(str **s, const char *d, size_t l) {
    if (!d || l == 0) {
        return -1;
    }

    if (str_get_len(*s) + l >= str_get_cap(*s)) {
        str *n = str_alloc(s, (l + (l >> 1)));

        if (!n) {
            return -1; 
        }

        *s = n;
    }

    memcpy(*s + str_get_len(*s), d, l + 1);
    str_set_len(s, str_get_len(*s) + l);

    return 0;
}

int str_push_s(str **s, const char *d) {
    if (!d) {
        return -1;
    }

    return str_push_l(s, d, strlen(d));
}

int str_undo_l(str **s, size_t l) {
    if (l > str_get_len(*s)) {
        return -1;
    }

    str_set_len(s, str_get_len(*s) - l);
    return 0;
}

int str_undo_s(str **s, const char *d) {
    if (!d) {
        return -1;
    }

    return str_undo_l(s, strlen(d));
}

int str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int l1 = vsnprintf(NULL, 0, f, ap2);
    va_end(ap2);

    if (l1 <= 0) {
        return -1;
    }

    if (str_get_len(*s) + (size_t) l1 >= str_get_cap(*s)) {
        str *n = str_alloc(s, (size_t) l1);

        if (!n) {
            return -1; 
        }

        *s = n;
    }

    int l2 = vsnprintf(*s + str_get_len(*s), (size_t) l1 + 1, f, ap);

    if (l1 == l2) {
        str_set_len(s, str_get_len(*s) + (size_t) l1);
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
        free((size_t *) *s - 2);
        *s = NULL;
    }
}

