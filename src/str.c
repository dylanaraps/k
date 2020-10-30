#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

#define HDR_LEN (sizeof (size_t) * 2)
#define CAP_OFF 2
#define LEN_OFF 1

str *str_init(size_t l) {
    size_t *s = malloc(HDR_LEN + l + 1);

    if (!s) {
        return NULL;
    }

    s[0] = s[1] = s[2] = 0;     

    return (char *) &s[CAP_OFF];
}

str *str_alloc(str **s, size_t l) {
    size_t *s2 = realloc((size_t *) *s + -CAP_OFF, 
        HDR_LEN + str_get_cap(s) + l + 1);

    if (!s2) {
        return NULL;
    }

    s2[0] += l + 1;

    return (char *) &s2[CAP_OFF];
}

int str_maybe_alloc(str **s, size_t l) {
    if (str_get_len(s) + l >= str_get_cap(s)) {
        str *n = str_alloc(s, (l + (l >> 1)));

        if (!n) {
            return -1; 
        }

        *s = n;
    }

    return 0;
}

int str_push_l(str **s, const char *d, size_t l) {
    if (!d || l == 0) {
        return -1;
    }

    if (str_maybe_alloc(s, l) < 0) {
        return -1;
    }

    memcpy(*s + str_get_len(s), d, l + 1);
    str_set_len(s, str_get_len(s) + l);

    return 0;
}

int str_push_s(str **s, const char *d) {
    if (!d) {
        return -1;
    }

    return str_push_l(s, d, strlen(d));
}

int str_undo_l(str **s, size_t l) {
    if (l > str_get_len(s)) {
        return -1;
    }

    str_set_len(s, str_get_len(s) - l);
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

    if (str_maybe_alloc(s, (size_t) l1) < 0) {
        return -1;
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

inline size_t str_get_cap(str **s) {
    return *s ? ((size_t *) *s)[-CAP_OFF] : 0;
}

inline size_t str_get_len(str **s) {
    return *s ? ((size_t *) *s)[-LEN_OFF] : 0;
}

inline void str_set_cap(str **s, size_t l) {
    ((size_t *) *s)[-CAP_OFF] = l;
}

inline void str_set_len(str **s, size_t l) {
    ((size_t *) *s)[-LEN_OFF] = l;
}

void str_free(str **s) {
    if (*s) {
        free((size_t *) *s - CAP_OFF);
        *s = NULL;
    }
}

