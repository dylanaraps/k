#include <stdio.h>

#include "log.h"
#include "find.h"
#include "util.h"
#include "pkg.h"

void pkg_init(package **pkg, char *pkg_name) {
    package *tmp = *pkg;
    package *new = xmalloc(sizeof(package)); 

    if (!pkg_name) {
        die("Package name is null");
    }

    /* implicitly initializes all fields */
    *new = (package) {
        .name = pkg_name,
    };

    pkg_find(new);

    if (!*pkg) {
        *pkg = new;

    } else {
        while (tmp->next) {
            tmp = tmp->next;
        }

        tmp->next = new;
        new->prev = tmp;
    }
}

void pkg_destroy(package *pkg) {
    package *tmp = pkg;
    int i;

    while (pkg) {
        tmp = pkg->next;

        for (i = 0; i < pkg->path_l; i++) {
            free(pkg->path[i]);
        }

        free(pkg->path);
        free(pkg);

        pkg = tmp;
    }

    free(pkg);
}
