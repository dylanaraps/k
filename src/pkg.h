#ifndef _KPKG_H
#define _KPKG_H

#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */

typedef struct package {
    char *name;

    char **path;
    int  path_l;

    char *ver;
    char *rel;

    char mak_dir[PATH_MAX];
    char pkg_dir[PATH_MAX];
    char tar_dir[PATH_MAX];

    char src_dir[PATH_MAX];
    char bin_dir[PATH_MAX];

    char **sum;
    char **src;
    char **des;
    int  src_l;
    int  sum_l;

    struct package *next;
    struct package *prev;
} package;

extern package *PKG;

void pkg_init(package **pkg, char *pkg_name);
void pkg_iter(package *pkg, void (*f)(package *pkg), const char *msg);
void pkg_destroy_all(void);
void pkg_null(package *pkg);

#endif
