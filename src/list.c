#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>

#include "log.h"
#include "pkg.h"
#include "util.h"
#include "list.h"

int pkg_list(const char *pkg_name) {
    return exists_at(DB_DIR, pkg_name, O_DIRECTORY);
}

void pkg_list_all(package *pkg) {
    package *tmp = pkg;
    struct dirent  **list;
    int err;
    int i;

    if (!pkg) {
        err = scandir(DB_DIR, &list, NULL, alphasort);

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

    for (tmp = pkg; tmp; tmp = tmp->next) {
        if (pkg_list(tmp->name) != 0) {
            die("[%s] Package not installed", tmp->name);
        }

        printf("%s %s %s\n", tmp->name, tmp->ver, tmp->rel);
    }
}
