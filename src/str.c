#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"

void str_cat(str *s, const char *p) {
    if (!p) {
        return;
    }

    size_t p_len = strlen(p);    

    if (!s->len || (s->len + p_len) >= s->cap) {
        s->cap += p_len * 2; 
        s->buf  = realloc(s->buf, s->cap);

        if (!s->buf) {
            perror("realloc");
            exit(1); 
        }
    }

    memcpy(s->buf + s->len, p, p_len + 1);

    s->len += p_len;
    s->buf[s->len] = 0;
}

void str_free(str *s) {
    free(s->buf);    
}
