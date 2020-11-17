/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "action.h"
#include "arr.h"
#include "download.h"
#include "error.h"
#include "file.h"
#include "pkg.h"

static size_t source_dest_path(struct state *s,
                          const char *n, const char *d, const char *f) {
    size_t mem_pre = buf_len(s->mem) + 1;
    buf_printf(&s->mem, "%c%s../../sources/%s/", 0, s->cache.dir, n);

    if (d) {
        buf_push_s(&s->mem, d);
        buf_rstrip(&s->mem, '/');
        buf_push_c(&s->mem, '/');
    }

    buf_push_s(&s->mem, strrchr(f, '/') + 1);
    return mem_pre;
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

        switch (pkg_source_type(p, s->mem)) {
            case SRC_URL:
                while (*f2 && *f2 == '/') f2++;

                char *dest = s->mem + source_dest_path(s, p->name, f2, s->mem);

                if (access(dest, F_OK) == 0) {
                    continue;
                }

                if (cache_mkdirat(s->cache.fd[CAC_SRC], p->name, f2) < 0) {
                    return -1;
                }

                if (download(s->mem, dest) < 0) {
                    return -1;
                }

                parsed++;
                break;

            case -1:
                err_no("source '%s' not found", s->mem);
                return -1;
        }
    }

    return parsed;
}

int action_download(struct state *s) {
    int err = 0;

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        FILE *src = pkg_fopen(s->pkgs[i], "sources", O_RDONLY, "r");

        if (!src && errno == ENOENT) {
            err("[%s] no sources file, skipping", s->pkgs[i]->name);
            continue;
        }

        if (!src) {
            err_no("failed to open sources file");
            err = -1;
            goto error;
        }

        msg("[%zu/%zu] downloading sources for [%s]",
            i + 1, arr_len(s->pkgs), s->pkgs[i]->name);

        int parsed = parse_source_file(s, s->pkgs[i], src);
        fclose(src);

        if (parsed == -1) {
            err = -1;
            goto error;
        }

        if (parsed == 0) {
            puts("nothing to do");
        }
    }

error:
    source_curl_cleanup();
    return err;
}

