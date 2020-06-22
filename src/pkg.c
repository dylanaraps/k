#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h> /* strlen */

#include "log.h"
#include "cache.h"
#include "find.h"
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

    state_init(new, "build", new->mak_dir);
    state_init(new, "extract", new->tar_dir);
    state_init(new, "pkg", new->pkg_dir);
    state_init(new, "../sources", new->src_dir);
    state_init(new, "../bin", new->bin_dir);

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

void pkg_iter(package *pkg, void (*f)(package *pkg), const char *msg) {
    package *tmp;

    (void)(msg);
    /* if (msg) { */
    /*     printf("\n\033[1m%s\033[m\n", msg); */
    /* } */

    for (tmp = pkg; tmp; tmp = tmp->next) {
        (*f)(tmp);
    }
}
