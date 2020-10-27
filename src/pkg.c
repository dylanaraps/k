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

FILE *pkg_fopen(struct pkg *p, const char *f) {
    int repo_fd = open(p->repo, O_RDONLY);    

    if (repo_fd == -1) {
        return NULL;
    }

    int pkg_fd = openat(repo_fd, p->name, O_RDONLY);
    close(repo_fd);

    if (pkg_fd == -1) {
        return NULL;
    }

    int fd = openat(pkg_fd, f, O_RDONLY);
    close(pkg_fd);

    if (fd == -1) {
        return NULL;
    }

    return fdopen(fd, "r");
}

int pkg_source(struct pkg *p) {
    FILE *f = pkg_fopen(p, "sources");

    if (!f) {
        return errno == ENOENT ? -2 : -1;
    }

    char *line = 0;

    while (getline(&line, &(size_t){0}, f) != -1) {
        if (line[0] == '#' || line[0] == '\n') {
            continue; 
        }

        char *src = strtok(line, " \n");
        char *des = strtok(NULL, " \n");

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

void pkg_free(struct pkg **p) {
    free((*p)->name); 
    free(*p);
    *p = NULL;
}

