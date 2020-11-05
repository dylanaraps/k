/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "list.h"
#include "str.h"
#include "action.h"

static int compare(void const *a, void const *b) {
    return strcmp(*(const char **) a, *(const char **) b);
}

static int pkg_list(str **buf, const char *pkg) {
    str_push_l(buf, "/var/db/kiss/installed/", 23);
    str_push_s(buf, pkg);
    str_push_l(buf, "/version", 8);

    FILE *ver = fopen((*buf)->buf, "r");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);

        } else {
            err_no("failed to open file '%s'", (*buf)->buf);
        }

        return -1;
    }

    str_set_len(*buf, 0);
    int ret = 0;

    if ((ret = str_getline(buf, ver)) == 0) {
        printf("%s %s\n", pkg, (*buf)->buf);

    } else {
        err_no("file read '...%s/version' failed", pkg);
    }

    fclose(ver);
    return ret;
}

static int pkg_list_all(str **buf, str **dir_buf) {
    DIR *db = opendir("/var/db/kiss/installed");

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    int err = 0;

    list pkgs;

    if ((err = list_init(&pkgs, 256)) < 0) {
        err("failed to allocate memory");
        goto error;
    }

    struct dirent *dp = 0;

    while ((dp = readdir(db))) {
        size_t len_pre = (*dir_buf)->len;

        str_push_s(dir_buf, dp->d_name);
        str_push_c(dir_buf, 0);

        list_push_b(&pkgs, (*dir_buf)->buf + len_pre);
    }

    list_sort(&pkgs, compare);

    for (size_t i = 2; i < pkgs.len; i++) {
        char *pkg = pkgs.arr[i];

        if ((err = pkg_list(buf, pkg)) < 0) {
            goto error;
        }

        // soft reset buffer
        str_set_len(*buf, 0);
    }

error:
    list_free(&pkgs);
    closedir(db);
    return err;
}

int action_list(int argc, char *argv[]) {
    str *buf = str_init(1024);

    if (!buf) {
        err("failed to allocate memory");
        return -ENOMEM;
    }

    int err = 0;

    for (int i = 2; i < argc; i++) {
        if ((err = pkg_list(&buf, argv[i])) < 0) {
            goto error;
        }

        // soft reset buffer
        str_set_len(buf, 0);
    }

    if (argc == 2) {
        str *dir_buf = str_init(1024);

        if (!dir_buf) {
            err("failed to allocate memory");
            err = -ENOMEM;
            goto error;
        }

        err = pkg_list_all(&buf, &dir_buf);
        str_free(&dir_buf);
    }

error:
    str_free(&buf);
    return err;
}

