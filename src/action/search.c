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

int action_search(buf **buf, int argc, char *argv[]) {
    buf_push_s(buf, getenv("KISS_PATH"));
    buf_push_c(buf, ':');
    buf_push_l(buf, "/var/db/kiss/installed", 22);

    list repos;
    glob_t g = {0};

    if (list_init(&repos, 12) < 0) {
        goto error;
    }

    for (char *t = strtok(*buf, ":"); t; t = strtok(NULL, ":")) {
        if (t[0] != '/') {
            err("invalid path '%s' in KISS_PATH", t);
            goto error;
        }

        list_push_b(&repos, t);
    }

    size_t len = buf_len(*buf);

    for (int i = 2; i < argc; i++) {
        for (size_t j = 0; j < repos.len; j++) {
            buf_push_s(buf, repos.arr[j]);
            buf_rstrip(buf, '/');
            buf_push_c(buf, '/');
            buf_push_s(buf, argv[i]);

            if (glob(*buf + len, g.gl_pathc ? GLOB_APPEND : 0, NULL, &g) < 0) {
                goto error;
            }

            buf_set_len(*buf, len);
        }

        if (g.gl_pathc == 0) {
            err("no search results for '%s'", argv[i]);
            goto error;
        }
    }

    for (size_t i = 0; i < g.gl_pathc; i++) {
        printf("%s\n", g.gl_pathv[i]);
    }

    list_free(&repos);
    globfree(&g);
    return 0;
error:
    list_free(&repos);
    globfree(&g);
    return -1;
}

