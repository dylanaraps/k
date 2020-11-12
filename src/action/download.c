/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <errno.h>

#include "arr.h"
#include "error.h"
#include "pkg.h"
#include "action.h"

int action_download(struct state *s) {
    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        FILE *src = pkg_fopen(s->pkgs[i]->repo_fd, s->pkgs[i]->name, "sources");

        switch (src ? 0 : errno) {
            case 0:
                break;

            case ENOENT:
                err("[%s] no sources file, skipping", s->pkgs[i]->name);
                continue;

            default:
                err_no("failed to open sources file");
                return -1;
        }

        for (; buf_getline(&s->mem, src, 256) == 0; buf_set_len(s->mem, 0)) {
            if (!*s->mem || *s->mem == '\n' || *s->mem == '#') {
                continue;
            }

            printf("%s\n", s->mem);
        }

        fclose(src);
    }

    return 0;
}

