#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"
#include "vec.h"

char *xgetenv(const char *s) {
    char *p = getenv(s);

    if (!p && !p[0]) {
        return NULL;
    }

    char *p2 = strdup(p);

    if (!p2) {
        perror("strdup");
        exit(1);
    }

    return p2;
}

int is_dir(const char *d) {
    struct stat s;

    int err = lstat(d, &s);

    if (err < 0) {
        return 1;
    }

    if (S_ISDIR(s.st_mode)) {
        return 0;
    }

    return 1;
}
