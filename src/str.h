#ifndef STR_H_
#define STR_H_

#include <stdio.h>

typedef struct str {
    size_t len; 
    size_t cap;
    char *buf;
} str;

void str_cat(str *, const char *);
void str_free(str *);

#endif
