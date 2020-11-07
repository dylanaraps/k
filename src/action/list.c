/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "error.h"
#include "list.h"
#include "str.h"
#include "action.h"

static int compare(void const *a, void const *b) {
    return strcmp(*(const char **) a, *(const char **) b);
}

static FILE *pkg_fopen(int repo_fd, const char *pkg, const char *file) {
    int pfd = openat(repo_fd, pkg, O_RDONLY);

    if (pfd == -1) {
        return NULL;
    }

    int ffd = openat(pfd, file, O_RDONLY);
    close(pfd);

    if (ffd == -1) {
        return NULL;
    }

    return fdopen(ffd, "r");
}

static int pkg_list(str **buf, int repo_fd, const char *pkg) {
    FILE *ver = pkg_fopen(repo_fd, pkg, "version");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);

        } else {
            err_no("[%s] failed to open version file", pkg);
        }

        return -1;
    }

    size_t len_pre = (*buf)->len;

    switch (str_getline(buf, ver, 32)) {
        case 0:
            printf("%s %s\n", pkg, (*buf)->buf + len_pre);
            str_set_len(*buf, len_pre);
            fclose(ver);
            return 0;

        default:
            err_no("[%s] failed to read version file", pkg);
            fclose(ver);
            return -1;
    }
}

static int pkg_list_all(str **buf, list *pkgs, int repo_fd) {
    DIR *db = fdopendir(repo_fd);

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    struct dirent *dp = 0;

    while ((dp = readdir(db))) {
        size_t len_pre = (*buf)->len;

        str_push_s(buf, dp->d_name);
        str_push_c(buf, 0);

        list_push_b(pkgs, (*buf)->buf + len_pre);
    }

    list_sort(pkgs, compare);

    // i = 2 skips '.' and '..'
    for (size_t i = 2; i < pkgs->len; i++) {
        if (pkg_list(buf, repo_fd, pkgs->arr[i]) < 0) {
            closedir(db);
            return -1;
        }
    }

    closedir(db);
    return 0;
}

int action_list(str **buf, int argc, char *argv[]) {
    int repo_fd = open("/var/db/kiss/installed", O_RDONLY);

    if (repo_fd == -1) {
        err_no("failed to open database");
        return -1;
    }

    for (int i = 2; i < argc; i++) {
        if (pkg_list(buf, repo_fd, argv[i]) < 0) {
            close(repo_fd);
            return -1;
        }
    }

    int ret = 0;

    if (argc == 2) {
        list pkgs;

        if ((ret = list_init(&pkgs, 256)) == 0) {
            ret = pkg_list_all(buf, &pkgs, repo_fd);

        } else {
            err("failed to allocate memory");
        }

        list_free(&pkgs);
    }

    // handled implicitly in pkg_list_all() via closedir();
    close(repo_fd);
    return 0;
}

