#ifndef KISS_STR_H_
#define KISS_STR_H_

#include <stdio.h>
#include <sys/types.h>

typedef struct str {
    size_t len;
    size_t cap;
    char *buf;
} str;

void str_push(str *, const char *);
void str_undo(str *, const char *);
void str_free(str *);

#endif
