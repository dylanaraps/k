#ifndef _KPKG_H
#define _KPKG_H

#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */

typedef struct package {
    char *name;
    char *ver;
    char *rel;

    /* locations */
    char **path;
    int  path_l;

    /* states */
    char mak_dir[PATH_MAX];
    char pkg_dir[PATH_MAX];
    char tar_dir[PATH_MAX];
    char  db_dir[PATH_MAX];

    /* caches */
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
void pkg_destroy(package *pkg);

#ifdef FREE_ON_EXIT
void pkg_destroy_all(void);
#endif

#endif
