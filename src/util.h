#include <stdlib.h>
#include <dirent.h>

void *xmalloc(size_t n);
void xchdir(const char *path);
void mkchdir(const char *path);
FILE *xfopen(const char *file, const char *p);
void copy_file(char *src, char *dest);
size_t strlcpy(char *d, const char *s, size_t n);
int ends_with(const char *str, const char *suf, size_t str_len, size_t suf_len);
