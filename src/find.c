#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */
#include <string.h> /* strcpy */

#include "log.h"
#include "repo.h"
#include "util.h"
#include "pkg.h"
#include "find.h"

void pkg_find(package *pkg) {
    char cwd[PATH_MAX];
    int i;

    pkg->path = xmalloc(REPO_LEN * sizeof(char *));

    for (i = 0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
            die("Repository not accessible (%s)", REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
            pkg->path[pkg->path_len] = xmalloc(PATH_MAX); 
            strcpy(pkg->path[pkg->path_len], getcwd(cwd, sizeof(cwd)));

            pkg->path_len++;
       }
    }

    if (pkg->path_len == 0) {
        die("Package (%s) does not exist", pkg->name);
    }
}
