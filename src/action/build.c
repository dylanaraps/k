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

static int pkg_verify_sources(buf *m, pkg *p) {
    FILE *f = pkg_fopen(p, "checksums", O_RDONLY, "r");

    if (!f && errno == ENOENT) {
        err_no("[%s] checksums file missing", p->name);
        return -1;
    }

    if (!f) {
        err_no("[%s] failed to open checksums file", p->name);
        return -1;
    }

    for (; buf_getline(&m, f, 256) == 0; buf_set_len(m, 0)) {
        if (m[0] == 'S' && m[1] == 'K' && m[2] == 'I' && m[3] == 'P' && !m[4]) {
            continue;
        }

        printf("%s\n", m);
    }

    fclose(f);
    return 0;
}

int action_build(struct state *s) {
    int ret = action_download(s);

    if (ret < 0) {
        return ret;
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        if ((ret = pkg_verify_sources(s->mem, s->pkgs[i])) < 0) {
            return ret;
        }
    }

    return 0;
}

