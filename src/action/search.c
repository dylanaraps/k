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
    glob_t g = { .gl_pathc = 0, };

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        size_t glob_pre = g.gl_pathc;

        for (size_t j = 0; j < arr_len(s->repos); j++) {
            buf_set_len(s->mem, 0);
            buf_printf(&s->mem, "%s/%s/",
                s->repos[j]->path, s->pkgs[i]->name);

            switch (glob(s->mem, g.gl_pathc ? GLOB_APPEND : 0, NULL, &g)) {
                case GLOB_NOSPACE:
                case GLOB_ABORTED:
                    err("glob encountered error with query '%s'", s->mem);
                    goto glob_error;
            }
        }

        if ((g.gl_pathc - glob_pre) == 0) {
            err("no search results for '%s'", s->pkgs[i]->name);
            goto glob_error;
        }
    }

    for (size_t i = 0; i < g.gl_pathc; i++) {
        puts(g.gl_pathv[i]);
    }

    globfree(&g);
    return 0;

glob_error:
    globfree(&g);
    return -1;
}

