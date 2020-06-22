#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */

#include "log.h"
#include "repo.h"
#include "strl.h"
#include "util.h"
#include "pkg.h"
#include "find.h"

/* todo: globbing */

void pkg_find(package *pkg) {
    char cwd[PATH_MAX];
    char *tmp;
    int i;
    size_t err;

    for (i = 0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
            die("[%s] Repository not accessible (%s)", pkg->name, REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
           tmp = getcwd(cwd, PATH_MAX);

           if (!tmp) {
               die("[%s] Repository not accessible (%s)", pkg->name, REPOS[i]);
           }

           err = strlcpy(pkg->path, tmp, PATH_MAX);

           if (err >= PATH_MAX) {
               die("strlcpy failed");
           }

           return;
       }
    }

    die("Package '%s' does not exist", pkg->name);
}

void pkg_paths(package *pkg) {
    int i;

    for (i = 0; i < REPO_LEN; i++) {
        pkg_find(pkg);
    }
}
