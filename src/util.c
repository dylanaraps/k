#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "str.h"
#include "util.h"

int PATH_prepend(const char *path, const char *var) {
    if (!path || !var) {
        return 1;
    }

    str *kiss_path = NULL;
    str_fmt(&kiss_path, "%s:%s", path, getenv(var));

    int err = setenv(var, kiss_path->buf, 1);

    str_free(&kiss_path);

    if (err == -1) {
        perror("setenv");
        return 1;
    }

    return 0;
}

int is_dir(const char *d) {
    struct stat s;

    if (lstat(d, &s) < 0) {
        return 1;
    }

    if (S_ISDIR(s.st_mode)) {
        return 0;
    }

    return 1;
}

char *path_basename(char *p) {
    if (!p) {
        return NULL;
    }

    size_t len = strlen(p);

    for (size_t i = 1; p[len - i] == '/'; i++) {
        p[len - i] = 0;
    }

    char *b = strrchr(p, '/');

    if (!b) {
        *p = '/';
        return p;
    }

    *b++ = 0;

    return b; // p now points to dirname
}

int mkdir_p(char *p, const mode_t m) {
    for (char *d = p + 1; *d; d++) {
        if (*d == '/') {
            *d = 0;

            if (mkdir(p, m) == -1 && errno != EEXIST) {
                return 1;
            }

            *d = '/';
        }
    }

    return 0;
}
