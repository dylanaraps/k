#include <stdio.h>  /* printf */
#include <unistd.h> /* chdir */
#include <dirent.h> /* scandir, opendir */

#include "log.h"
#include "list.h"

int pkg_list(package *pkg) {
    /* todo: prepend KISS_PATH */
    if (chdir("/var/db/kiss/installed") != 0) {
        die("[%s] Repository is not accessible", pkg->name);
    }

    if (chdir(pkg->name) != 0) {
        return 1;
    }

    return 0;
}

void pkg_list_all(package *pkg) {
    struct dirent  **list;
    int err;
    int i;

    if (!pkg) {
        /* todo: prepend KISS_PATH */
        err = scandir("/var/db/kiss/installed", &list, NULL, alphasort);

        if (err == -1) {
            die("Package DB not accessible");
        }

        // '2' skips '.'/'..'.
        for (i = 2; i < err; i++) {
            pkg_init(&pkg, list[i]->d_name);
            free(list[i]);
        }

        free(list);
    }

    for (; pkg; pkg = pkg->next) {
        err = pkg_list(pkg);

        if (err == 0) {
            printf("%s %s %s\n", pkg->name, pkg->ver, pkg->rel);
        } else {
            die("[%s] Package not installed", pkg->name);
        }
    }
}

