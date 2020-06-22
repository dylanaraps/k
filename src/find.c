#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <fcntl.h>  /* openat */

#include "log.h"
#include "repo.h"
#include "strl.h"
#include "util.h"
#include "pkg.h"
#include "find.h"

void pkg_find(package *pkg, const int all) {
    int i;
    int err;

    for (i = 0; i < REPO_LEN; i++) {
        err = exists_at(REPOS[i], pkg->name, O_DIRECTORY);

        if (err == 0) {
            err = snprintf(pkg->path, PATH_MAX, "%s/%s", REPOS[i], pkg->name);

            if (err >= PATH_MAX) {
                die("strlcpy failed");
            }

            if (!all) {
                return;
            }

            printf("%s\n", pkg->path);
        }
    }

    if (!all) {
        die("Package '%s' does not exist", pkg->name);
    }
}

void pkg_paths(package *pkg) {
    pkg_find(pkg, 1);
}
