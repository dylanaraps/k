#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h> /* strlen */

#include "log.h"
#include "find.h"
#include "version.h"
#include "util.h"
#include "pkg.h"

package *PKG;

void pkg_init(package **pkg, char *pkg_name) {
    package *tmp = *pkg;
    package *new = xmalloc(sizeof(package)); 

    /* initializes all fields */
    *new = (package) {
        .name = strdup(pkg_name),
    };

    if (!new->name) {
        die("Package name is null");
    }

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

        free(PKG->name);
        free(PKG->ver);
        free(PKG->rel);

        for (i = 0; i < PKG->path_l; i++) {
            free(PKG->path[i]);
        }
        free(PKG->path);

        for (i = 0; i < PKG->src_l; i++) {
            free(PKG->src[i]);
            free(PKG->des[i]);
        }
        free(PKG->src);
        free(PKG->des);

        free(PKG);

        PKG = tmp;
    }

    free(PKG);
}
