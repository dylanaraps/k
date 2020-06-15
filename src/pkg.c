#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>

#include "pkg.h"

void pkg_load(package **head, char *pkg_name) {
    package *new_pkg = (package*) malloc(sizeof(package));    
    package *last = *head;

    if (!new_pkg) {
        printf("error: Failed to allocate memory\n");
        exit(1);
    }

    new_pkg->next = NULL;
    new_pkg->name = pkg_name;
    new_pkg->path = pkg_find(pkg_name);

    if (!*head) {
        new_pkg->prev = NULL;
        *head = new_pkg;
        return;
    }

    while (last->next) {
        last = last->next;
    }

    last->next = new_pkg;
    new_pkg->prev = last;
}

void cache_init(void) {
    HOME      = getenv("HOME");
    CAC_DIR   = getenv("XDG_CACHE_HOME");
    char cwd[PATH_MAX];

    if (!HOME || HOME[0] == '\0') {
        printf("HOME directory is NULL\n");
        exit(1);
    }

    if (!CAC_DIR || CAC_DIR[0] == '\0') {
        chdir(HOME);

        mkdir(".cache", 0777);

        if (chdir(".cache") != 0) {
            goto err;
        }
        
        CAC_DIR = strdup(getcwd(cwd, sizeof(cwd)));
    }

    mkdir(CAC_DIR, 0777);

    if (chdir(CAC_DIR) != 0) {
        goto err;
    }

    mkdir("kiss", 0777);

    if (chdir("kiss") != 0) {
        goto err;
    }

    CAC_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    mkdir("build", 0777);
    mkdir("pkg", 0777);
    mkdir("extract", 0777);
    mkdir("sources", 0777);
    mkdir("logs", 0777);  

    if (chdir("build") != 0) {
        goto err;
    }
    MAK_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../pkg") != 0) {
        goto err;
    }
    PKG_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../extract") != 0) {
        goto err;
    }
    TAR_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../sources") != 0) {
        goto err;
    }
    SRC_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../logs") != 0) {
        goto err;
    }
    LOG_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../bin") != 0) {
        goto err;
    }
    BIN_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    chdir(PWD);
    return;

err:
    printf("%s\n", "Failed to create cache directory\n");
    exit(1);
}
