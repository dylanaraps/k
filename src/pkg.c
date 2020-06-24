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

char DB_DIR[PATH_MAX] = "/var/db/kiss/installed";
static package *PKG;

package *pkg_init(package **pkg, char *pkg_name) {
    package *tmp = PKG;
    package *new;

    if (pkg_have(pkg_name) == 0) {
        return NULL;
    }

    new = xmalloc(sizeof(package));

    /* initializes all fields */
    *new = (package) {
        .name = strdup(pkg_name),
    };

    if (!new->name) {
        die("Package name is null");
    }

    pkg_find(new, 0);
    pkg_version(new);

    if (!*pkg) {
        *pkg = PKG = new;

    } else {
        while (tmp->next) {
            tmp = tmp->next;
        }

        tmp->next = new;
        new->prev = tmp;
    }

    return new;
}

void pkg_state_init(package *pkg) {
    state_init(pkg, "build",      pkg->mak_dir);
    state_init(pkg, "extract",    pkg->tar_dir);
    state_init(pkg, "pkg",        pkg->pkg_dir);
    state_init(pkg, "../sources", pkg->src_dir);
    state_init(pkg, "../bin",     pkg->bin_dir);
}

void pkg_iter(package *pkg, void (*f)(package *pkg)) {
    package *tmp;

    for (tmp = pkg; tmp; tmp = tmp->next) {
        (*f)(tmp);
    }
}

int pkg_have(char *pkg_name) {
    package *tmp = PKG;

    while (tmp) {
        if (strcmp(tmp->name, pkg_name) == 0) {
            return 0;
        }

        tmp = tmp->next;
    }

    return 1;
}

static void pkg_free(package *pkg) {
    int i;

    free(pkg->name);
    free(pkg->ver);
    free(pkg->rel);
    free(pkg->path);

    for (i = 0; i < pkg->src_l; i++) {
        free(pkg->src[i]);
        free(pkg->des[i]);
    }
    free(pkg->src);
    free(pkg->des);

    for (i = 0; i < pkg->sum_l; i++) {
        free(pkg->sum[i]);
    }
    free(pkg->sum);

    for (i = 0; i < pkg->dep_l; i++) {
        free(pkg->dep[i]);
        free(pkg->dep_type[i]);
    }
    free(pkg->dep);
    free(pkg->dep_type);

    free(pkg);
}

void pkg_destroy(package *pkg) {
    if (!PKG || !pkg) {
        return;
    }

    if (PKG == pkg) {
        PKG = pkg->next;
    }

    if (pkg->next) {
        pkg->next->prev = pkg->prev;
    }

    if (pkg->prev) {
        pkg->prev->next = pkg->next;
    }

    pkg_free(pkg);
}

void pkg_destroy_all(void) {
    while (PKG) {
        pkg_destroy(PKG);
    }
}
