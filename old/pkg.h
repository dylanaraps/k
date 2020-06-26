#ifndef _KPKG_H
#define _KPKG_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <limits.h> /* PATH_MAX */

typedef struct package {
    char *name;
    char *ver;
    char *rel;

    /* location */
    char path[PATH_MAX];

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
} package;

extern char DB_DIR[22];
extern package *PKG;

package pkg_init(char *pkg_name);
void pkg_iter(package *pkg, void (*f)(package *pkg));
int pkg_have(char *pkg_name);
void pkg_state_init(package *pkg);
void pkg_destroy(package *pkg);
void pkg_destroy_all(void);

#endif
