#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */
#include <glob.h>   /* glob */

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

    pkg->path = xmalloc(REPO_LEN * sizeof(char *));

    for (i = 0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
            die("[%s] Repository not accessible (%s)", pkg->name, REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
           tmp = getcwd(cwd, PATH_MAX);

           if (!tmp) {
               die("[%s] Repository not accessible (%s)", pkg->name, REPOS[i]);
           }

           pkg->path[pkg->path_l] = xmalloc(PATH_MAX); 
           err = strlcpy(pkg->path[pkg->path_l], tmp, PATH_MAX);

           if (err > PATH_MAX) {
               die("strlcpy truncated path");
           }

           pkg->path_l++;
       }
    }

    if (pkg->path_l == 0) {
        die("Package '%s' does not exist", pkg->name);
    }
}
