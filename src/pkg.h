#ifndef KISS_PKG_H_
#define KISS_PKG_H_

#include <stdlib.h>

struct version {
    char *ver;
    char *rel;
};

struct sources {
    char *path;
    char *dest;
};

struct depends {
    char *name;
    char *type;
};

typedef struct pkg {
    char *name;
    char *path; // path to location on disk.

    struct depends *depends;
    struct sources *sources;
    struct version version;
} pkg;

pkg *pkg_init(const char *name);
pkg *pkg_init_die(const char *name);
void pkg_free(pkg **p);

void pkg_version(str **s, const char *name, const char *repo);
void pkg_list_print(str **s, const char *name, const char *repo);
void pkg_list_installed(str **s, const char *repo);

#endif
