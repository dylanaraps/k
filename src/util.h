#include <stdlib.h> /* size_t */
#include <stdio.h>  /* FILE */

void *xmalloc(size_t n);
void xfree(void *p);
int cntchr(const char *str, int chr);
int cntlines(FILE *file);
int strsuf(const char *str, const char *suf, size_t suf_len);
int exists_at(const char *path, const char *file, const int flags);
