#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "vec.h"
#include "cache.h"
#include "util.h"
#include "repo.h"
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

int pkg_list(int repo_fd, char *pkg) {
    int fd = openat(repo_fd, pkg, O_RDONLY);

    if (fd == -1) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);
            return -1;
        }

        err_no("failed to open pkg '%s'", pkg);
        return -1;
    }

    FILE *ver = fopenat(fd, "version", O_RDONLY, "r");
    close(fd);

    if (!ver) {
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
    close((*p)->repo);
    free((*p)->name); 
    free(*p);
    *p = NULL;
}

