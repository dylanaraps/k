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
    SRC_URL,
    SRC_GIT,
    SRC_ABS,
    SRC_REL,
};

static size_t source_dest(struct state *s, char *name, char *des) {
    size_t mem_pre = buf_len(s->mem) + 1;

    buf_printf(&s->mem, "%c%s../../sources/%s/", 0, s->cache.dir, name);

    if (des) {
        while (*des && *des == '/') des++;

        buf_push_s(&s->mem, des);
        buf_rstrip(&s->mem, '/');
        buf_push_c(&s->mem, '/');
    }

    return mem_pre;
}

static int source_type(pkg *p, char *src) {
    if (src[0] == 'g' && src[1] == 'i' && src[2] == 't' && src[3] == '+') {
        return SRC_GIT;

    } else if (src[0] == '/') {
        return access(src, F_OK) == 0 ? SRC_ABS : -1;

    } else if (strstr(src, "://")) {
        return SRC_URL;

    } else if (pkg_faccessat(p->repo_fd, p->name, src) == 0) {
        return SRC_REL;
    }

    return -1;
}

static int download(const char *url, const char *dest) {
    FILE *src_file = fopen(dest, "w");

    if (!src_file) {
        err_no("failed to open source '%s'", dest);
        return -1;
    }

    int err = source_download(url, src_file);
    fclose(src_file);

    if (err < 0) {
        unlink(dest);
    }

    return err;
}

static int parse_source_file(struct state *s, pkg *p, FILE *f) {
    int parsed = 0;

    for (; buf_getline(&s->mem, f, 256) == 0; buf_set_len(s->mem, 0)) {
        if (!*s->mem || *s->mem == '\n' || *s->mem == '#') {
            continue;
        }

        char *f2 = s->mem + buf_scan(&s->mem, 0, ' ');

        switch (source_type(p, s->mem)) {
            case SRC_URL: {
                char *dest = s->mem + source_dest(s, p->name, f2);

                if (mkdir_p(dest, 0755) < 0) {
                    return -1;
                }

                if (buf_push_s(&s->mem, strrchr(s->mem, '/') + 1) == -EINVAL) {
                    return -1;
                }

                if (access(dest, F_OK) == 0) {
                    continue;
                }

                if (download(s->mem, dest) < 0) {
                    return -1;
                }

                parsed++;
                break;
            }

            case -1:
                err_no("source '%s' not found", s->mem);
                return -1;
        }
    }

    return parsed;
}

int action_download(struct state *s) {
    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        FILE *src = pkg_fopen(s->pkgs[i]->repo_fd, s->pkgs[i]->name, "sources");

        if (!src && errno == ENOENT) {
            err("[%s] no sources file, skipping", s->pkgs[i]->name);
            continue;
        }

        if (!src) {
            err_no("failed to open sources file");
            return -1;
        }

        printf("[%s] downloading sources\n", s->pkgs[i]->name);
        int parsed = parse_source_file(s, s->pkgs[i], src);
        fclose(src);

        if (parsed == -1) {
            source_curl_cleanup();
            return -1;
        }

        if (parsed == 0) {
            printf("nothing to do\n");
        }
    }

    source_curl_cleanup();
    return 0;
}

