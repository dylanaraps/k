#ifndef KISS_PKG_H
#define KISS_PKG_H

struct pkg {
    char *name;
    char *repo;
};

struct pkg *pkg_create(const char *name);
int pkg_source(struct pkg *p);
int pkg_list(int repo_fd, char *pkg);
void pkg_free(struct pkg **p);

#endif
