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
#include "sha256.h"
#include "action.h"

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
                src = pkg_fopen(p->repo_fd, p->name, s->mem, O_RDONLY, "r");
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
        parsed++;

        fprintf(d, "%s\n", hash_string);
        puts(hash_string);
    }

    return parsed;
}

int action_checksum(struct state *s) {
    int err = action_download(s);

    if (err < 0) {
        return err;
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        FILE *src = pkg_fopen(s->pkgs[i]->repo_fd,
            s->pkgs[i]->name, "sources", O_RDONLY, "r");

        if (!src && errno == ENOENT) {
            err("[%s] no sources file, skipping", s->pkgs[i]->name);
            continue;
        }

        if (!src) {
            err_no("failed to open sources file");
            return -1;
        }

        FILE *chk = pkg_fopen(s->pkgs[i]->repo_fd,
            s->pkgs[i]->name, "checksums", O_RDWR | O_CREAT, "w");

        if (!chk) {
            err_no("[%s] failed to open checksums file", s->pkgs[i]->name);
            fclose(src);
            return -1;
        }

        printf("[%s] generating checksums\n", s->pkgs[i]->name);
        int parsed = parse_source_file(s, s->pkgs[i], src, chk);
        fclose(src);
        fclose(chk);

        if (parsed == -1) {
            return -1;
        }

        if (parsed == 0) {
            puts("nothing to do");
        }
    }

    return 0;
}

