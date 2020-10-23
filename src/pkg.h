#ifndef KISS_PKG_H_
#define KISS_PKG_H_

#include <stdlib.h>

typedef struct pkg {
    str *name;
    str *repo; 
    str *version;

    str **source;
    size_t *src_off;

    str **depend;
    size_t *dep_off;
} pkg;

pkg *pkg_create(void);
pkg *pkg_init(const char *name);
void pkg_init_name(pkg **p, const char *name);
void pkg_init_repo(pkg **p, const char *repo);
void pkg_init_sources(pkg **p);
int pkg_init_version(pkg **p);
void pkg_init_depends(pkg **p);
void pkg_free(pkg **p);

FILE *pkg_fopen(pkg **p, const char *file, size_t len);

void pkg_list_print(str **s, const char *name, const char *repo);
void pkg_list_installed(str **s, const char *repo);

#endif
