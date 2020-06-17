#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>

#include "list.h"
#include "log.h"
#include "pkg.h"

void pkg_list(char *pkg_name) {
    struct version version;
    char *path;
    char cwd[PATH_MAX];

    SAVE_CWD;

    if (chdir(PKG_DB) != 0) {
        log_error("Package DB not accessible");
    }

    if (chdir(pkg_name) != 0) {
        PKG = pkg_name;
        log_error("not installed");

    } else {
        path = getcwd(cwd, sizeof(cwd)); 
        version = pkg_version(path);
        printf("%s %s %s\n", pkg_name, version.version, version.release);
    }

    LOAD_CWD;
}

void pkg_list_all(void) {
    struct version version;
    struct dirent  **list;
    int tot;

    if (chdir(PKG_DB) != 0) {
        log_error("Package DB not accessible");
    }

    tot = scandir(".", &list, NULL, alphasort);

    if (tot == -1) {
        log_error("Package DB not accessible");
    }

    // '2' skips '.'/'..'.
    for (int i = 2; i < tot; i++) {
        if (chdir(list[i]->d_name) == 0) {
            version = pkg_version(list[i]->d_name);    

            printf("%s %s %s\n", list[i]->d_name, \
                    version.version, version.release);
        }

        chdir(PKG_DB);
    }
}

