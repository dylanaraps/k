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
#include "arr.h"
#include "buf.h"
#include "repo.h"
#include "pkg.h"
#include "action.h"

static int compare(void const *a, void const *b) {
    return strcmp(*(const char **) a, *(const char **) b);
}

static int pkg_list(buf **buf, int repo_fd, const char *pkg) {
    FILE *ver = pkg_fopen(repo_fd, pkg, "version");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);

        } else {
            err_no("[%s] failed to open version file", pkg);
        }

        return -1;
    }

    size_t len_pre = buf_len(*buf);

    switch (buf_getline(buf, ver, 32)) {
        case 0:
            printf("%s %s\n", pkg, *buf + len_pre);
            buf_set_len(*buf, len_pre);
            fclose(ver);
            return 0;

        default:
            err_no("[%s] failed to read version file", pkg);
            fclose(ver);
            return -1;
    }
}

static int pkg_list_all(buf **buf, char **pkgs, int repo_fd) {
    DIR *db = fdopendir(repo_fd);

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    for (struct dirent *dp; (dp = readdir(db)); ) {
        size_t len_pre = buf_len(*buf);

        buf_push_s(buf, dp->d_name);
        buf_push_c(buf, 0);

        arr_push_b(pkgs, *buf + len_pre);
    }

    arr_sort(pkgs, compare);

    // i = 2 skips '.' and '..'
    for (size_t i = 2; i < arr_len(pkgs); i++) {
        if (pkg_list(buf, repo_fd, pkgs[i]) < 0) {
            closedir(db);
            return -1;
        }
    }

    closedir(db);
    return 0;
}

int action_list(int argc, char *argv[]) {
    buf *buf = buf_alloc(0, 1024);

    if (!buf) {
        return -ENOMEM;
    }

    int err = 0;
    struct repo *db = repo_open_db();

    if (!db) {
        err_no("failed to open database");
        err = -1;
        goto repo_error;
    }

    for (int i = 2; i < argc; i++) {
        if ((err = pkg_list(&buf, db->fd, argv[i])) < 0) {
            goto pkg_error;
        }
    }

    if (argc == 2) {
        char **pkgs = arr_alloc(0, 256);

        if (pkgs) {
            err = pkg_list_all(&buf, pkgs, db->fd);

        } else {
            err("failed to allocate memory");
        }

        arr_free(pkgs);
    }

pkg_error:
    buf_free(&buf);
repo_error:
    repo_free(db);

    return err;
}

