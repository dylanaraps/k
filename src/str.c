#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "str.h"

str *str_init(size_t l) {
    str *s = malloc(sizeof (str) + l + 1);

    if (s) {
         s->cap = l + 1;
         s->len = 0;
         s->err = STR_OK;
        *s->buf = 0;
    }

    return s;
}

void str_alloc(str **s, size_t l) {
    if ((*s)->err == STR_OK && l != 0) {
        str *s2 = realloc(*s, sizeof (str) + (*s)->cap + l);

        if (s2) {
            (*s) = s2;
            (*s)->cap += l;
        } else {
            (*s)->err = STR_ENOMEM;
        }
    }
}

void str_push_c(str **s, int c) {
    if ((*s)->len + 1 >= (*s)->cap) {
        str_alloc(s, ((*s)->cap + ((*s)->cap >> 1)));
    }

    if ((*s)->err == STR_OK) {
        (*s)->buf[(*s)->len] = c;
        (*s)->buf[(*s)->len += 1] = 0;
    }
}

void str_push_l(str **s, const char *d, size_t l) {
    if (d && l > 0) {
        if (((*s)->len + l) >= (*s)->cap) {
            str_alloc(s, (l + (l >> 1)));
        }

        if ((*s)->err == STR_OK) {
            for (size_t i = 0; i < l; i++) {
                str_push_c(s, d[i]);
            }
        }
    } else {
        (*s)->err = STR_EINVAL;
    }
}

void str_push_s(str **s, const char *d) {
    if (d) {
        if ((*s)->err == STR_OK) {
            size_t l = 0;

            while (d[l]) l++;

            str_push_l(s, d, l);
        }
    } else {
        (*s)->err = STR_EINVAL;
    }
}

void str_undo_l(str **s, size_t l) {
    if (l <= (*s)->len) {
        if ((*s)->err == STR_OK) {
            (*s)->buf[(*s)->len -= l] = 0;
        }
    } else {
        (*s)->err = STR_EINVAL;
    }
}

void str_undo_s(str **s, const char *d) {
    if (d) {
        if ((*s)->err == STR_OK) {
            size_t l = 0;

            while (d[l]) l++;

            str_undo_l(s, l);
        }
    } else {
        (*s)->err = STR_EINVAL;
    }
}

void str_getline(str **s, FILE *f) {
    if (f) {
        int c;

        while ((c = fgetc(f)) != '\n' && c != EOF) {
            str_push_c(s, c);
        }

        (*s)->err = c == EOF ? STR_EOF : (*s)->err;

    } else {
        (*s)->err = STR_EINVAL;
    }
}

str *str_dup(str **s) {
    if ((*s)->err == STR_OK) {
        str *n = str_init((*s)->len);

        if (n) {
            str_push_l(&n, (*s)->buf, (*s)->len);

            if (n->err == STR_OK) {
                return n;
            }
        }

        str_free(n);
    }

    return NULL;
}

void str_vprintf(str **s, const char *f, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);

    int l1 = vsnprintf(NULL, 0, f, ap2);

    va_end(ap2);

    if (l1 > 0 && (*s)->err == STR_OK) {
        if (((*s)->len + (size_t) l1) >= (*s)->cap) {
            str_alloc(s, (size_t) l1);
        }

        if ((*s)->err == STR_OK) {
            int l2 = vsnprintf((*s)->buf + (*s)->len,
                (size_t) l1 + 1, f, ap);

            if (l1 == l2) {
                (*s)->len += (size_t) l1;
                return;
            }
        }
    }

    (*s)->err = STR_ERROR;
}

void str_printf(str **s, const char *f, ...) {
    va_list ap;
    va_start(ap, f);
    str_vprintf(s, f, ap);
    va_end(ap);
}

void str_path_normalize(str **s) {
    for (; (*s)->buf[(*s)->len - 1] == '/';
           (*s)->buf[--(*s)->len] = 0);
}

void str_free(str *s) {
    if (s) {
        free(s);
    }
}

