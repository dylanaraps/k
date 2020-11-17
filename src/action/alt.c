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

int action_alt(struct state *s) {
    struct repo *db = repo_open_db();

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    int ret = 0;

    switch (arr_len(s->argv)) {
        case 0:
            // list all
            break;

        case 2:
            // swap
            break;

        default:
            err("invalid arguments passed");
            ret = -1;
            goto error;
    }

error:
    repo_free(db);
    return ret;
}

