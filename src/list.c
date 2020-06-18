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

    if (chdir(pkg->name) != 0) {
        PKG = pkg->name;
        log_error("not installed");
    }

    printf("%s %s %s\n", pkg->name, pkg->ver, pkg->rel);

    LOAD_CWD;
}

void pkg_list_all(void) {
    package *head = NULL;
    struct dirent  **list;
    int tot;

    xchdir(PKG_DB);
    tot = scandir(".", &list, NULL, alphasort);

    if (tot == -1)
        log_error("Package DB not accessible");

    // '2' skips '.'/'..'.
    for (int i = 2; i < tot; i++) {
        if (chdir(list[i]->d_name) == 0)
            pkg_load(&head, list[i]->d_name);

        xchdir(PKG_DB);
    }

    for (package *tmp = head; tmp; tmp = tmp->next)
        pkg_list(tmp);
}

