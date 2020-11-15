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
    pkg const *p1 = *(pkg const **) a;
    pkg const *p2 = *(pkg const **) b;

    return strcmp(p1->name, p2->name);
}

static int pkg_list(buf **buf, int repo_fd, const char *pkg) {
    FILE *ver = pkg_fopen(repo_fd, pkg, "version", O_RDONLY, "r");

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
            fprintf(stdout, "%s %s %s\n", pkg,
                *buf + len_pre,
                *buf + buf_scan(buf, len_pre, ' '));
            buf_set_len(*buf, len_pre);
            fclose(ver);
            return 0;

        default:
            err_no("[%s] failed to read version file", pkg);
            fclose(ver);
            return -1;
    }
}

int action_list(struct state *s) {
    int err = 0;
    struct repo *db = repo_open_db();

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    DIR *d = fdopendir(db->fd);

    if (!d) {
        err_no("failed to open database");
        return -1;
    }

    if (arr_len(s->pkgs) == 0) {
        for (struct dirent *dp; (dp = readdir(d)); ) {
            if (dp->d_name[0] == '.' && (!dp->d_name[1] ||
               (dp->d_name[1] == '.' && !dp->d_name[2]))) {
                continue;
            }

            if ((err = state_init_pkg(s, dp->d_name)) < 0) {
                goto error;
            }
        }

        arr_sort(s->pkgs, compare);
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        if ((err = pkg_list(&s->mem, db->fd, s->pkgs[i]->name)) < 0) {
            break;
        }
    }

error:
    closedir(d);
    repo_free(db);
    return err;
}

