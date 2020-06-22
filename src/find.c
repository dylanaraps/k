#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */
#include <fcntl.h>  /* openat */

#include "log.h"
#include "repo.h"
#include "strl.h"
#include "util.h"
#include "pkg.h"
#include "find.h"

/* todo: globbing */

void pkg_find(package *pkg, const int all) {
    int i;
    int err;
    int fd;

    for (i = 0; i < REPO_LEN; i++) {
        fd = open(REPOS[i], O_RDONLY | O_DIRECTORY);

        if (fd == -1) {
            die("[%s] Repository not accessible (%s)", pkg->name, REPOS[i]);
        }

        err = openat(fd, pkg->name, O_RDONLY);
        close(fd);

        if (err != -1) {
            close(err);

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
