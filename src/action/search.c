/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <glob.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "error.h"
#include "list.h"
#include "buf.h"
#include "action.h"

static int get_repositories(buf **buf) {
    if (buf_push_s(buf, getenv("KISS_PATH")) == -ENOMEM) {
        err("failed to allocate memory");
        return -ENOMEM;
    }

    buf_push_c(buf, ':');

    if (buf_push_s(buf, getenv("KISS_ROOT")) == -ENOMEM) {
        err("failed to allocate memory");
        return -ENOMEM;
    }

    buf_rstrip(buf, '/');
    buf_push_c(buf, '/');
    buf_push_l(buf, "var/db/kiss/installed", 21);

    return 0;
}

static int get_repo_list(list *repos, char *buf) {
    if (list_init(repos, 12) < 0) {
        err("failed to allocate memory");
        return -ENOMEM;
    }

    for (char *t = strtok(buf, ":"); t; t = strtok(NULL, ":")) {
        if (t[0] != '/') {
            err("invalid path '%s' in KISS_PATH", t);
            return -1;
        }

        list_push_b(repos, t);
    }

    return 0;
}

int action_search(buf **buf, int argc, char *argv[]) {
    int err = 0;

    if ((err = get_repositories(buf)) < 0) {
        err("failed to get repository list");
        return -1;
    }

    list repos;

    if ((err = get_repo_list(&repos, *buf)) < 0) {
        err("failed to initialize repository list");
        goto list_error;
    }

    glob_t g = { .gl_pathc = 0, };

    for (int i = 2; i < argc; i++) {
        size_t glob_pre = g.gl_pathc;

        for (size_t j = 0, len = buf_len(*buf); j < repos.len; j++) {
            buf_push_s(buf, repos.arr[j]);
            buf_rstrip(buf, '/');
            buf_push_c(buf, '/');
            buf_push_s(buf, argv[i]);
            buf_push_c(buf, '/');

            err = glob(*buf + len, g.gl_pathc ? GLOB_APPEND : 0, NULL, &g);

            if (err < 0) {
                err("glob encountered error with query '%s'", *buf + len);
                goto glob_error;
            }

            buf_set_len(*buf, len);
        }

        if ((g.gl_pathc - glob_pre) == 0) {
            err("no search results for '%s'", argv[i]);
            err = -1;
            goto glob_error;
        }
    }

    for (size_t i = 0; i < g.gl_pathc; i++) {
        puts(g.gl_pathv[i]);
    }

glob_error:
    globfree(&g);
list_error:
    list_free(&repos);
    return err;
}

