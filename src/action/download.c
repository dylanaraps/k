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

enum sources {
    SRC_GIT,
    SRC_CAC,
    SRC_ABS,
    SRC_REL,
};

static size_t source_contruct(struct state *s, size_t i, char *des) {
    buf_push_c(&s->mem, 0);

    size_t mem_pre = buf_len(s->mem);

    buf_push_s(&s->mem, s->cache.dir);
    buf_push_l(&s->mem, "../../sources/", 14);
    buf_push_s(&s->mem, s->pkgs[i]->name);
    buf_push_c(&s->mem, '/');

    if (des) {
        while (*des && *des == '/') des++;

        buf_push_s(&s->mem, des);
        buf_rstrip(&s->mem, '/');
        buf_push_c(&s->mem, '/');
    }

    return mem_pre;
}

static int source_type(struct state *s, size_t i, char *src) {
    if (src[0] == 'g' && src[1] == 'i' && src[2] == 't' && src[3] == '+') {
        return SRC_GIT;

    } else if (strstr(src, "://")) {
        return SRC_CAC;

    } else if (src[0] == '/') {
        if (access(src, F_OK) == 0) {
            return SRC_ABS;
        }
        return -1;

    } else if (pkg_faccessat(s->pkgs[i]->repo_fd, s->pkgs[i]->name, src) == 0) {
        return SRC_REL;
    }

    return -1;
}

static int parse_source_line(struct state *s, size_t i) {
    char *f1 = strtok(s->mem, " 	\r\n");
    char *f2 = strtok(NULL,   " 	\r\n");

    switch (source_type(s, i, f1)) {
        case SRC_CAC: {
            size_t len_pre = source_contruct(s, i, f2);

            if (mkdir_p(s->mem + len_pre, 0755) < 0) {
                return -1;
            }

            if (buf_push_s(&s->mem, strrchr(f1, '/') + 1) == -EINVAL) {
                return -1;
            }

            if (access(s->mem + len_pre, F_OK) == 0) {
                return 0;
            }

            FILE *src_file = fopen(s->mem + len_pre, "w");

            if (!src_file) {
                err_no("failed to open source '%s'", s->mem + len_pre);
                return -1;
            }

            int err = source_download(f1, src_file);
            fclose(src_file);

            if (err < 0) {
                unlink(s->mem + len_pre);
                return -1;
            }
            break;
        }

        case -1:
            err_no("source '%s' not found", f1);
            return -1;
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

