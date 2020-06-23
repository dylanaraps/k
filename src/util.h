#include <stdlib.h> /* size_t */
#include <stdio.h>  /* FILE */

void *xmalloc(size_t n);
void xstrlcpy(char *dst, const char *src, size_t dsize);
void xsnprintf(char *str, size_t size, const char *fmt, ...);
int cntchr(const char *str, int chr);
int cntlines(FILE *file);
int strsuf(const char *str, const char *suf, size_t suf_len);
int exists_at(const char *path, const char *file, const int flags);
void split_in_two(char *line, const char *delim, char **one, char **two);
FILE *fopenat(const char *path, const char *file, const char *mode);
