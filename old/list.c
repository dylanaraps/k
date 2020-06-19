#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>

#include "list.h"
#include "log.h"
#include "util.h"
#include "version.h"
#include "pkg.h"

void pkg_list(package *pkg) {
    SAVE_CWD;
    xchdir(PKG_DB);

    if (chdir(pkg->name) != 0)
        die("[%s] not installed", pkg->name);

    printf("%s %s %s\n", pkg->name, pkg->ver, pkg->rel);
    LOAD_CWD;
}

void pkg_list_all(package *pkg) {
    struct dirent  **list;
    int tot;

    tot = scandir(PKG_DB, &list, NULL, alphasort);

    if (tot == -1)
        die("Package DB not accessible");

    // '2' skips '.'/'..'.
    for (int i = 2; i < tot; i++) {
        if (opendir(list[i]->d_name) != 0)
            pkg_init(&pkg, list[i]->d_name);

        free(list[i]);
        xchdir(PKG_DB);
    }
    free(list);

    for (; pkg; pkg = pkg->next)
        pkg_list(pkg);
}

