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

int action_alt(struct state *s) {
    switch (arr_len(s->argv)) {
        case 0:
            break;

        case 2:
            break;

        default:
            err("invalid arguments passed");
            return -1;
    }

    return 0;
}

