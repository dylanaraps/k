#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "arr.h"
#include "buf.h"
#include "pkg.h"

pkg *pkg_alloc(const char *name) {
    pkg *n = malloc(sizeof *n);

    if (n) {
        n->repo_fd = 0;
        n->name = strdup(name);

        if (n->name) {
            return n;
        }

        free(n);
    }

    return NULL;
}

void pkg_free(pkg *p) {
    free(p->name);
    free(p);
}

void pkg_free_all(pkg **p) {
    for (size_t i = 0; i < arr_len(p); i++) {
        pkg_free(p[i]);
    }

    arr_free(p);
}

FILE *pkg_fopen(int repo_fd, const char *pkg, const char *file) {
    int pfd = openat(repo_fd, pkg, O_RDONLY);

    if (pfd == -1) {
        return NULL;
    }

    int ffd = openat(pfd, file, O_RDONLY);
    close(pfd);

    if (ffd == -1) {
        return NULL;
    }

    return fdopen(ffd, "r");
}

int pkg_faccessat(int repo_fd, const char *pkg, const char *file) {
    int pfd = openat(repo_fd, pkg, O_RDONLY);

    if (pfd == -1) {
        return -1;
    }

    int ret = faccessat(pfd, file, F_OK, 0);
    close(pfd);
    return ret;
}

