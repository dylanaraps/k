/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "arr.h"
#include "download.h"
#include "error.h"
#include "file.h"
#include "pkg.h"
#include "action.h"

static int parse_source_line(struct state *s, size_t i) {
    char *f1 = strtok(s->mem, " 	\r\n");
    char *f2 = strtok(NULL,   " 	\r\n");
    char *bn = strrchr(f1, '/');

    if (strncmp(f1, "git+", 4) == 0) {
        printf("found git %s\n", f1);

    } else if (strstr(f1, "://")) {
        buf_push_c(&s->mem, 0);

        size_t mem_pre = buf_len(s->mem);

        buf_push_s(&s->mem, s->cache.dir);
        buf_push_l(&s->mem, "../../sources/", 14);
        buf_push_s(&s->mem, s->pkgs[i]->name);
        buf_push_c(&s->mem, '/');

        if (f2) {
            while (*f2 && *f2 == '/') f2++;

            buf_push_s(&s->mem, f2);
            buf_rstrip(&s->mem, '/');
            buf_push_c(&s->mem, '/');

            if (mkdir_p(s->mem + mem_pre, 0755) < 0) {
                return -1;
            }
        }

        buf_push_s(&s->mem, bn + 1);

        if (access(s->mem + mem_pre, F_OK) == 0) {
            printf("found cache %s\n", bn + 1);

        } else {
            FILE *src_file = fopen(s->mem + mem_pre, "w");

            if (!src_file) {
                err_no("failed to open source '%s'", s->mem + mem_pre);
                return -1;
            }

            int err = source_download(f1, src_file);
            fclose(src_file);

            if (err < 0) {
                unlink(s->mem + mem_pre);
                return -1;
            }
        }
    }

    return 0;
}

static int parse_source_file(struct state *s, size_t i, FILE *f) {
    for (; buf_getline(&s->mem, f, 256) == 0; buf_set_len(s->mem, 0)) {
        if (!*s->mem || *s->mem == '\n' || *s->mem == '#') {
            continue;
        }

        if (parse_source_line(s, i) < 0) {
            return -1;
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

        printf("[%s] downloading sources\n", s->pkgs[i]->name);
        int err = parse_source_file(s, i, src);
        fclose(src);

        if (err == -1) {
            source_curl_cleanup();
            return -1;
        }
    }

    return 0;
}

