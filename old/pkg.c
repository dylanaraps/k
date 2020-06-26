#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "cache.h"
#include "find.h"
#include "find.h"
#include "version.h"
#include "util.h"
#include "vec.h"
#include "pkg.h"

char DB_DIR[22] = "/var/db/kiss/installed";
package *PKG;

package pkg_init(char *pkg_name) {
    package new;

    if (!pkg_name) {
        die("Package name is null");
    }

    /* initializes all fields */
    new = (package) {
        .name = strdup(pkg_name),
    };

    if (!new.name) {
        die("Package name is null");
    }

    pkg_find(&new, 0);
    pkg_version(&new);

    return new;
}

void pkg_state_init(package *pkg) {
    state_dir_init(pkg, "build",   pkg->mak_dir);
    state_dir_init(pkg, "extract", pkg->tar_dir);
    state_dir_init(pkg, "pkg",     pkg->pkg_dir);
    cache_dir_init(pkg, "sources", pkg->src_dir);
    cache_dir_init(pkg, "bin",     pkg->bin_dir);
}

void pkg_iter(package *pkg, void (*f)(package *pkg)) {
    for (size_t i = 0; i < vec_size(pkg); ++i) {
        (*f)(&pkg[i]);
    }
}
