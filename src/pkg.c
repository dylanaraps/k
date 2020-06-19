#include <stdio.h>

#include "log.h"
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
