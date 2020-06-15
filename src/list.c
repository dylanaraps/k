#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>

#include "list.h"
#include "pkg.h"

void pkg_list(char *pkg_name) {
    struct version version;
    char *db = "/var/db/kiss/installed"; 
    char *path;
    char cwd[PATH_MAX];

    if (chdir(db) != 0) {
        printf("error: Package db not accessible\n");
        exit(1);
    }

    if (chdir(pkg_name) != 0) {
        printf("error: Package %s not installed\n", pkg_name);
        exit(1);

    } else {
        path = getcwd(cwd, sizeof(cwd)); 
        version = pkg_version(path);
        printf("%s %s %s\n", pkg_name, version.version, version.release);
    }

    chdir(PWD);
}

void pkg_list_all(void) {
    struct version version;
    struct dirent  **list;
    int tot;
    char db[] = "/var/db/kiss/installed";

    if (chdir(db) != 0) {
        printf("error: Failed to access package db\n");
        exit(1);
    }

    tot = scandir(".", &list, NULL, alphasort);

    if (tot == -1) {
        printf("error: Failed to access package db\n");
        exit(1);
    }

    // '2' skips '.'/'..'.
    for (int i = 2; i < tot; i++) {
        if (chdir(list[i]->d_name) == 0) {
            version = pkg_version(list[i]->d_name);    

            printf("%s %s %s\n", list[i]->d_name, \
                    version.version, version.release);
        }

        chdir(db);
    }
}

