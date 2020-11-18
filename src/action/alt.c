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
    for (size_t i = 0, l = strlen(n), f = 0; i < l; i++) {
        if (n[i] == '>') {
            if (f++ == 0) {
                putchar(' ');
            }

            putchar('/');

        } else {
            putchar(n[i]);
        }
    }

    putchar('\n');
}

static int list_alts(struct state *s, const char *db) {
    DIR *d = opendir(db);

    if (!d) {
        err_no("failed to open database");
        return -1;
    }

    for (struct dirent *dp; (dp = readdir(d)); ) {
        if (dp->d_name[0] == '.' && (!dp->d_name[1] ||
           (dp->d_name[1] == '.' && !dp->d_name[2]))) {
            continue;
        }

        arr_push_b(s->argv, dp->d_name);
    }
    arr_sort(s->argv, qsort_cb_str);

    for (size_t i = 0; i < arr_len(s->argv); i++) {
        print_alt(s->argv[i]);
    }

    closedir(d);
    return 0;
}

int action_alt(struct state *s) {
    struct repo *db = repo_open_db("choices");

    if (!db) {
        err_no("failed to open alt database");
        return -1;
    }

    int ret = 0;

    switch (arr_len(s->argv)) {
        case 0:
            if ((ret = list_alts(s, db->path)) < 0) {
                err_no("failed to list alts");
            }
            break;

        case 2:
            // swap
            break;

        default:
            err("invalid arguments passed");
            ret = -1;
            goto error;
    }

error:
    repo_free(db);
    return ret;
}

