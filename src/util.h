#include <stdlib.h> /* size_t */
#include <stdio.h>  /* FILE */

void *xmalloc(size_t n);
void xfree(void *p);
int cntchr(const char *str, int chr);
int cntlines(FILE *file);
void xsnprintf(char *str, size_t size, const char *fmt, ...);
int strsuf(const char *str, const char *suf, size_t suf_len);
int exists_at(const char *path, const char *file, const int flags);
FILE *fopenat(const char *path, const char *file, const char *mode);
