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

int action_checksum(struct state *s) {
    int err = action_download(s);

    if (err < 0) {
        return err;
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {

    }

    return 0;
}

