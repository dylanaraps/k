/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "action.h"
#include "arr.h"
#include "download.h"
#include "error.h"
#include "file.h"
#include "pkg.h"
#include "sha256.h"
#include "util.h"

static void print_alt(const char *n) {
    for (size_t f = 0; *n; n++) {
        if (*n == '>') {
            if (!f++) {
                putchar(' ');
            }

            putchar('/');

        } else {
            putchar(*n);
        }
    }

    putchar('\n');
}

static int list_alts(struct state *s, const char *db) {
    DIR *d = opendir(db);

    if (!d) {
        err_no("failed to open alt database");
        return -1;
    }

    for (struct dirent *dp; (dp = read_dir(d)); ) {
        arr_push_b(s->argv, dp->d_name);
    }
    arr_sort(s->argv, qsort_cb_str);

    for (size_t i = 0; i < arr_len(s->argv); i++) {
        print_alt(s->argv[i]);
    }

    closedir(d);
    return 0;
}

static int swap_alt(struct state *s, const char *pkg, const char *alt) {
    if (!repo_has_pkg(s->repos[1], pkg)) {
        err("package '%s' not installed", pkg);
        return -1;
    }

    buf_set_len(s->mem, 0);
    buf_push_s(&s->mem, pkg);
    buf_push_s(&s->mem, alt);
    buf_fr_cg(&s->mem, 0, '/', '>');

    struct stat sb;

    if (fstatat(s->repos[0]->fd, s->mem, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
        err("alternative '%s %s' does not exist", pkg, alt);
        return -1;
    }

    if (access(alt, F_OK) == 0) {
        // find owner of file
        // copy / -> choices
        // update manifest of owner
    }

    // move choices -> /
    // update manifest of choice

    return 0;
}

int action_alt(struct state *s) {
    if (repo_open_db_push(s->repos, "choices") == -1) {
        err_no("failed to open alt database");
        return -1;
    }

    int ret = 0;

    if (arr_len(s->argv) == 0) {
        ret = list_alts(s, s->repos[0]->path);

    } else if (arr_len(s->argv) == 1 &&
              (s->argv[0][0] == '-' && !s->argv[0][1])) {
        // swap stdin

    } else if (arr_len(s->argv) == 2) {
        if (s->argv[1][0] != '/' || !strstr(s->argv[1], "/")) {
            err("second argument ('%s') is invalid", s->argv[1]);
            return -1;
        }

        if ((ret = repo_open_db_push(s->repos, "installed")) == -1) {
            err_no("failed to open pkg database");
            return -1;
        }

        ret = swap_alt(s, s->argv[0], s->argv[1]);

    } else {
        err_no("invalid arguments");
        ret = -1;
    }

    return ret;
}

