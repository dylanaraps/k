#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "str.h"

void str_alloc(str *s, size_t len) {
    if (!s->len || (s->len + len) >= s->cap) {
        s->cap += len;
        s->buf  = realloc(s->buf, s->cap);

        if (!s->buf) {
            perror("realloc");
            exit(1);
        }
    }
}

void str_push(str *s, const char *p) {
    if (!p || !p[0]) {
        return;
    }

    size_t p_len = strlen(p);

    str_alloc(s, p_len * 2);
    memcpy(s->buf + s->len, p, p_len + 1);

    s->len += p_len;
    s->buf[s->len] = 0;
}

void str_undo(str *s, const char *p) {
    if (!p || !p[0]) {
        return;
    }

    s->buf[s->len -= strlen(p)] = 0;
}

void str_free(str *s) {
    free(s->buf);
}
