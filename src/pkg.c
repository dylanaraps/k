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

int pkg_source(struct pkg *p) {
    int repo_fd = open(p->repo, O_RDONLY);

    if (repo_fd == -1) {
        err("failed to open repository '%s': %s", p->repo, strerror(errno));
        return -1;
    }

    FILE *f = pkg_fopen(repo_fd, p->name, "sources");

    if (!f) {
        return errno == ENOENT ? -2 : -1;
    }

    char *line = 0;

    while (getline(&line, &(size_t){0}, f) != -1) {
        if (line[0] == '#' || line[0] == '\n') {
            continue; 
        }

        char *src = strtok(line, " \n");
        /* char *des = strtok(NULL, " \n"); */

        if (!src) {
            err("[%s] invalid sources file", p->name);
            return -1;
        }

        if (strncmp(src, "git+", 4) == 0) {
            msg("[%s] found git %s", p->name, src);    

        } else if (strstr(src, "://")) {

        }
    }

    free(line);
    fclose(f);

    return 0;
}

int pkg_list(int repo_fd, char *pkg) {
    FILE *ver = pkg_fopen(repo_fd, pkg, "version");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);
            return -1;
        }

        err("failed to open pkg '%s': %s", pkg, strerror(errno));
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

