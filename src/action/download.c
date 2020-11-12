/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "arr.h"
#include "error.h"
#include "pkg.h"
#include "action.h"

static int parse_source_file(struct state *s, size_t i, FILE *f) {
    for (; buf_getline(&s->mem, f, 256) == 0; buf_set_len(s->mem, 0)) {
        if (!*s->mem || *s->mem == '\n' || *s->mem == '#') {
            continue;
        }

        char *f1 = strtok(s->mem, " 	\r\n");
        char *f2 = strtok(NULL,   " 	\r\n");
        char *bn = strrchr(f1, '/');

        if (strncmp(f1, "git+", 4) == 0) {
            printf("found git %s\n", f1);

        } else if (strstr(f1, "://")) {
            int cac = openat(s->cache.fd[CAC_SRC],
                s->pkgs[i]->name, O_RDONLY);

            if (cac == -1) {
                err_no("failed to open sources directory");
                return -1;
            }

            size_t mem_pre = buf_len(s->mem);

            if (buf_push_s(&s->mem, f2) == 0) {
                buf_rstrip(&s->mem, '/');
                buf_push_c(&s->mem, '/');
            }
            buf_push_s(&s->mem, bn + 1);

            if (faccessat(cac, s->mem + mem_pre, F_OK, 0) == 0) {
                printf("found cache %s\n", s->mem + mem_pre);
            }

            close(cac);
        }
    }

    return 0;
}

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

        int err = parse_source_file(s, i, src);
        fclose(src);

        if (err == -1) {
            err_no("failed to parse sources file");
            return -1;
        }
    }

    return 0;
}

