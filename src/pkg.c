#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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

FILE *pkg_fopen(pkg *p, const char *f, int M, const char *m) {
    int pfd = openat(p->repo_fd, p->name, O_RDONLY);

    if (pfd == -1) {
        return NULL;
    }

    int ffd = openat(pfd, f, M, 0644);
    close(pfd);

    if (ffd == -1) {
        return NULL;
    }

    return fdopen(ffd, m);
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

int pkg_source_type(pkg *p, char *src) {
    if (src[0] == 'g' && src[1] == 'i' && src[2] == 't' && src[3] == '+') {
        return SRC_GIT;
    }

    if (src[0] == '/') {
        return access(src, F_OK) == 0 ? SRC_ABS : -1;
    }

    if (strstr(src, "://")) {
        return SRC_URL;
    }

    if (pkg_faccessat(p->repo_fd, p->name, src) == 0) {
        return SRC_REL;
    }

    return -1;
}

int pkg_sort_name(void const *a, void const *b) {
    pkg const *p1 = *(pkg const **) a;
    pkg const *p2 = *(pkg const **) b;

    return strcmp(p1->name, p2->name);
}
