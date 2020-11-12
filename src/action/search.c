/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <glob.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "error.h"
#include "arr.h"
#include "buf.h"
#include "repo.h"
#include "action.h"

int action_search(int argc, char *argv[]) {
    buf *buf = buf_alloc(0, 1024);

    if (!buf) {
        return -ENOMEM;
    }

    int err = 0;
    struct repo **repos = arr_alloc(0, 12);

    if (!repos) {
        err("failed to allocate memory");
        err = -1;
        goto repo_error;
    }

    if ((err = repo_open_PATH(repos, getenv("KISS_PATH"))) < 0) {
        err_no("failed to read KISS_PATH");
        goto repo_error;
    }

    glob_t g = { .gl_pathc = 0, };

    for (int i = 2; i < argc; i++) {
        size_t glob_pre = g.gl_pathc;

        for (size_t j = 0, len = buf_len(buf); j < arr_len(repos); j++) {
            buf_push_s(&buf, repos[j]->path);
            buf_rstrip(&buf, '/');
            buf_push_c(&buf, '/');
            buf_push_s(&buf, argv[i]);
            buf_push_c(&buf, '/');

            switch (glob(buf + len, g.gl_pathc ? GLOB_APPEND : 0, NULL, &g)) {
                case GLOB_NOSPACE:
                case GLOB_ABORTED:
                    err("glob encountered error with query '%s'", buf + len);
                    goto glob_error;

                case GLOB_NOMATCH:
                default:
                    break;
            }

            buf_set_len(buf, len);
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
repo_error:
    repo_free_all(repos);

    buf_free(&buf);
    return err;
}

