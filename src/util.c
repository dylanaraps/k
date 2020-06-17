#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    p = malloc(n);

    if (!p)
        log_error("Failed to allocate memory");

    return p;
}

void xchdir(const char *path) {
    int ret;

    if (!path)
        log_error("Directory %s not accessible", path);

    ret = chdir(path);
    
    if (ret != 0)
        log_error("Directory %s not accessible", path);
}

void copy_file(char *src, char *dest) {
    FILE *in;
    FILE *out;
    int err;
    int buf_len = 4096;
    char buffer[buf_len];

    in = fopen(src, "r");

    if (!in)
        log_error("File not accessible %s\n", src);

    out = fopen(dest, "w");

    if (!out)
        log_error("Cannot copy file %s\n", src);

    while (1) {
        err = fread(buffer, 1, buf_len, in); 

        if (err == -1)
            log_error("File not accessible %s\n", src);

        if (err == 0)
            break;

        err = fwrite(buffer, 1, buf_len, out);

        if (err == -1)
            log_error("Cannot copy file %s\n", src);
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
