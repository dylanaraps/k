#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>   /* malloc, size_t */
#include <stdio.h>    /* printf */
#include <limits.h>   /* PATH_MAX  */
#include <string.h>   /* strncpy */
#include <sys/stat.h> /* mkdir */
#include <errno.h>    /* errno, EEXIST, S_IRWXU */
#include <unistd.h>   /* access */
#include <fcntl.h>    /* open */

#include "log.h"
#include "strl.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    if (n == 0) {
        die("Empty memory allocation");
    }

    p = malloc(n);

    if (!p) {
        die("Failed to allocate memory");
    }

    return p;
}

int cntchr(const char *str, int chr) {
    const char *tmp = str;
    int i = 0;

    for (; tmp[i]; tmp[i] == chr ? i++ : *tmp++);

    return i;
}

int cntlines(FILE *file) {
    char *line = 0;
    int i = 0;

    while (getline(&line, &(size_t){0}, file) != -1) {
        if (line[0] != '#' && line[0] != '\n') {
           i++;
        }
    }
    rewind(file);
    free(line);

    return i;
}

int strsuf(const char *str, const char *suf, size_t suf_len) {
    if (!str || !suf || !suf_len) {
       return -1;
    }

    return strncmp(&str[strlen(str) - suf_len], suf, suf_len);
}

int exists_at(const char *path, const char *file, const int flags) {
    int fd;
    int re;

    fd = open(path, O_RDONLY | O_DIRECTORY);

    if (fd == -1) {
        return 1;
    }

    re = openat(fd, file, O_RDONLY | flags);

    close(fd);

    if (re == -1) {
        return 1;
    }

    close(re);
    return 0;
}
