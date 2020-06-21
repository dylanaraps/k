#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    p = malloc(n);

    if (!p)
        die("Failed to allocate memory");

    return p;
}

void xchdir(const char *path) {
    int ret;

    if (!path)
        die("Directory is null");

    ret = chdir(path);

    if (ret != 0)
        die("Directory %s not accessible", path);
}

void mkchdir(const char *path) {
    if (!path)
        die("Directory is null");

    mkdir(path, 0777);
    xchdir(path);
}

FILE *xfopen(const char *file, const char *p) {
     FILE *f;

     if (!file || !p)
         die("File is null");

     f = fopen(file, p);

     if (!f)
         die("Failed to open file %s", file);

     return f;
}

int ends_with(const char *str, const char *suf, size_t str_len, size_t suf_len) {
    if (!str || !suf || suf_len > str_len)
       return 0;

    return strncmp(str + str_len - suf_len, suf, suf_len) == 0;
}

void copy_file(char *src, char *dest) {
    FILE *in;
    FILE *out;
    int err;
    int buf_len = 4096;
    char buffer[buf_len];

    in  = xfopen(src,  "r");
    out = xfopen(dest, "w");

    while (1) {
        err = fread(buffer, 1, buf_len, in);

        if (err == -1)
            die("File not accessible %s\n", src);

        if (err == 0)
            break;

        err = fwrite(buffer, 1, buf_len, out);

        if (err == -1)
            die("Cannot copy file %s\n", src);
    }

    fclose(in);
    fclose(out);
}

size_t strlcpy(char *d, const char *s, size_t n) {
	char *d0 = d;
	size_t *wd;
	const size_t *ws;

	if (!n--) goto finish;
	if (((uintptr_t)s & (sizeof(size_t)-1)) ==
        ((uintptr_t)d & (sizeof(size_t)-1))) {
		for (; ((uintptr_t)s & (sizeof(size_t)-1)) && n &&
                (*d=*s); n--, s++, d++);
		if (n && *s) {
			wd=(void *)d; ws=(const void *)s;
			for (; n>=sizeof(size_t) &&
                !(((*ws)-((size_t)-1/UCHAR_MAX)) & ~(*ws) &
                    (((size_t)-1/UCHAR_MAX) * (UCHAR_MAX/2+1)));
            n-=sizeof(size_t), ws++, wd++) *wd = *ws;
			d=(void *)wd; s=(const void *)ws;
		}
	}
	for (; n && (*d=*s); n--, s++, d++);
	*d = 0;
finish:
	return d-d0 + strlen(s);
}
