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

int action_search(struct state *s) {
    int err = 0;
    glob_t g = { .gl_pathc = 0, };

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        size_t glob_pre = g.gl_pathc;

        for (size_t j = 0, l = buf_len(s->mem); j < arr_len(s->repos); j++) {
            buf_push_s(&s->mem, s->repos[j]->path);
            buf_rstrip(&s->mem, '/');
            buf_push_c(&s->mem, '/');
            buf_push_s(&s->mem, s->pkgs[i]->name);
            buf_push_c(&s->mem, '/');

            switch (glob(s->mem + l, g.gl_pathc ? GLOB_APPEND : 0, NULL, &g)) {
                case GLOB_NOSPACE:
                case GLOB_ABORTED:
                    err("glob encountered error with query '%s'", s->mem + l);
                    goto glob_error;

                case GLOB_NOMATCH:
                default:
                    break;
            }

            buf_set_len(s->mem, l);
        }

        if ((g.gl_pathc - glob_pre) == 0) {
            err("no search results for '%s'", s->pkgs[i]->name);
            err = -1;
            goto glob_error;
        }
    }

    for (size_t i = 0; i < g.gl_pathc; i++) {
        puts(g.gl_pathv[i]);
    }

glob_error:
    globfree(&g);
    return err;
}

