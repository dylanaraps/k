#ifndef KISS_PKG_H
#define KISS_PKG_H

struct pkg {
    char *name;
    char *repo;
};

struct pkg *pkg_create(const char *name);
FILE *pkg_fopen(struct pkg *p, const char *f);
int pkg_source(struct pkg *p);
void pkg_free(struct pkg **p);

#endif
