#ifndef _KPKG_H
#define _KPKG_H

typedef struct package {
    char *name;    

    char **path;
    int  path_l;

    struct package *next;
    struct package *prev;
} package;

extern package *PKG;

void pkg_init(package **pkg, char *pkg_name);
void pkg_destroy_all(void);

#endif
