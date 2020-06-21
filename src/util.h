#include <stdlib.h> /* size_t */
#include <stdio.h>  /* FILE */

void *xmalloc(size_t n);
int cntchr(const char *str, int chr);
int cntlines(FILE *file);
int strsuf(const char *str, const char *suf, size_t str_len, size_t suf_len);
void mkdir_p(const char *dir);
