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

static int parse_source_file(struct state *s, pkg *p, FILE *f) {
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
                src = pkg_fopen(p->repo_fd, p->name, s->mem);
                break;

            case SRC_URL:
                while (*f2 && *f2 == '/') f2++;
                src = cache_fopen(s->cache.fd[CAC_SRC],
                    p->name, f2, strrchr(s->mem, '/') + 1);
                break;

            case SRC_GIT:
                continue;
        }

        if (!src) {
            err_no("failed to open source '%s'", s->mem);
            return -1;
        }

        unsigned char hash[SHA256_DIGEST_LENGTH];
        sha256_file(hash, src);
        fclose(src);
        char hash_string[65];
        sha256_to_string(hash, hash_string);
        parsed++;

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
        FILE *src = pkg_fopen(s->pkgs[i]->repo_fd, s->pkgs[i]->name, "sources");

        if (!src && errno == ENOENT) {
            err("[%s] no sources file, skipping", s->pkgs[i]->name);
            continue;
        }

        if (!src) {
            err_no("failed to open sources file");
            return -1;
        }

        printf("[%s] generating checksums\n", s->pkgs[i]->name);
        int parsed = parse_source_file(s, s->pkgs[i], src);
        fclose(src);

        if (parsed == -1) {
            return -1;
        }

        if (parsed == 0) {
            puts("nothing to do");
        }
    }

    return 0;
}

