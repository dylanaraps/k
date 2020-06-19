#include <stdio.h>

#include "log.h"
#include "find.h"
#include "version.h"
#include "util.h"
#include "pkg.h"

package *PKG;

void pkg_init(package **pkg, const char *pkg_name) {
    package *tmp = *pkg;
    package *new = xmalloc(sizeof(package)); 

    if (!pkg_name) {
        die("Package name is null");
    }

    /* initializes all fields */
    *new = (package) {
        .name = pkg_name,
    };

    pkg_find(new);
    pkg_version(new);

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

void pkg_destroy_all(void) {
    package *tmp = PKG;
    int i;

    while (PKG) {
        tmp = PKG->next;

        free(PKG->ver);
        free(PKG->rel);

        for (i = 0; i < PKG->path_l; i++) {
            free(PKG->path[i]);
        }

        free(PKG->path);
        free(PKG);

        PKG = tmp;
    }

    free(PKG);
}
