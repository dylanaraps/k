#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "vec.h"
#include "util.h"
#include "pkg.h"

struct pkg *pkg_create(const char *name) {
    struct pkg *p = malloc(sizeof *p);    

    if (p) {
        p->repo = 0;
        p->name = strdup(name);

        if (p->name) {
            return p;
        }

        free(p);
    }

    return NULL;
}

FILE *pkg_fopen(int repo_fd, char *pkg, const char *file) {
    int pkg_fd = openat(repo_fd, pkg, O_RDONLY);

    if (pkg_fd == -1) {
        return NULL;
    }

    int fd = openat(pkg_fd, file, O_RDONLY);
    close(pkg_fd);

    if (fd == -1) {
        return NULL;
    }

    return fdopen(fd, "r");
}

int pkg_list(int repo_fd, char *pkg) {
    FILE *ver = pkg_fopen(repo_fd, pkg, "version");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);
            return -1;
        }

        err_no("failed to open pkg '%s'", pkg);
        return -1;
    }

    fputs(pkg, stdout);
    putchar(' ');
    int err = file_print_line(ver);
    fclose(ver);
    return err;
}

void pkg_free(struct pkg **p) {
    free((*p)->name); 
    free(*p);
    *p = NULL;
}

