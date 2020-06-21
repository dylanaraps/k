#include <stdio.h>  /* printf */
#include <unistd.h> /* chdir */
#include <dirent.h> /* scandir, opendir */

#include "log.h"
#include "list.h"

void pkg_list(package *pkg) {
    /* todo: prepend KISS_PATH */
    if (chdir("/var/db/kiss/installed") != 0) {
        die("[%s] Repository is not accessible", pkg->name);
    }

    if (chdir(pkg->name) != 0) {
        die("[%s] Package not installed", pkg->name);
    }

    printf("%s %s %s\n", pkg->name, pkg->ver, pkg->rel);
}

void pkg_list_all(package *pkg) {
    struct dirent  **list;
    int tot;

    /* todo: prepend KISS_PATH */
    tot = scandir("/var/db/kiss/installed", &list, NULL, alphasort);

    if (tot == -1) {
        die("Package DB not accessible");
    }

    // '2' skips '.'/'..'.
    for (int i = 2; i < tot; i++) {
        pkg_init(&pkg, list[i]->d_name);
        free(list[i]);
    }

    free(list);
    pkg_iter(pkg, pkg_list, NULL);
}

