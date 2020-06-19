#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */
#include <string.h> /* strncpy */

#include "log.h"
#include "repo.h"
#include "util.h"
#include "pkg.h"
#include "find.h"

void pkg_find(package *pkg) {
    char cwd[PATH_MAX];
    char *tmp;
    int i;

    pkg->path = xmalloc(REPO_LEN * sizeof(char *));

    for (i = 0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
            die("Repository not accessible (%s)", REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
           tmp = getcwd(cwd, PATH_MAX);

           if (!tmp) {
               die("Repository not accessible");
           }

           pkg->path[pkg->path_l] = xmalloc(PATH_MAX); 
           strncpy(pkg->path[pkg->path_l], tmp, PATH_MAX);

           pkg->path_l++;
       }
    }

    if (pkg->path_l == 0) {
        die("Package (%s) does not exist", pkg->name);
    }
}
