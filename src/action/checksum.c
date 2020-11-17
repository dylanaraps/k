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
#include "sha256.h"

static int parse_source_file(struct state *s, pkg *p, FILE *f, FILE *d) {
    int parsed = 0;

    for (; buf_getline(&s->mem, f, 256) == 0; buf_set_len(s->mem, 0)) {
        if (!*s->mem || *s->mem == '\n' || *s->mem == '#') {
            continue;
        }

        char *f2 = s->mem + buf_scan(&s->mem, 0, ' ');
        FILE *src = 0;

        switch (pkg_source_type(p, s->mem)) {
            case SRC_ABS:
                src = fopen(s->mem, O_RDONLY);
                break;

            case SRC_REL:
                src = pkg_fopen(p, s->mem, O_RDONLY, "r");
                break;

            case SRC_URL:
                while (*f2 && *f2 == '/') f2++;
                src = cache_fopen(s->cache.fd[CAC_SRC],
                    p->name, f2, strrchr(s->mem, '/') + 1, O_RDONLY, "r");
                break;

            case SRC_GIT:
                continue;
        }

        if (!src) {
            err_no("failed to open source '%s'", s->mem);
            return -1;
        }

        unsigned char hash[SHA256_LEN];
        sha256_file(hash, src);
        fclose(src);
        char hash_string[65];
        sha256_to_string(hash, hash_string);
        fprintf(d, "%s\n", hash_string);
        puts(hash_string);
        parsed++;
    }

    return parsed;
}

int action_checksum(struct state *s) {
    int ret = action_download(s);

    if (ret < 0) {
        return ret;
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        FILE *src = pkg_fopen(s->pkgs[i], "sources", O_RDONLY, "r");

        if (!src && errno == ENOENT) {
            err("[%s] no sources file, skipping", s->pkgs[i]->name);
            continue;
        }

        if (!src) {
            err_no("failed to open sources file");
            return -1;
        }

        FILE *chk = pkg_fopen(s->pkgs[i], "checksums", O_RDWR | O_CREAT, "w");

        if (!chk) {
            err_no("[%s] failed to open checksums file", s->pkgs[i]->name);
            fclose(src);
            return -1;
        }

        msg("[%zu/%zu] generating checksums for [%s]",
            i + 1, arr_len(s->pkgs), s->pkgs[i]->name);

        ret = parse_source_file(s, s->pkgs[i], src, chk);

        fclose(src);
        fclose(chk);

        switch (ret) {
            case 0:
                puts("nothing to do");
                break;

            case -1:
                return -1;

            default:
                puts("updated checksums file");
        }
    }

    return 0;
}

