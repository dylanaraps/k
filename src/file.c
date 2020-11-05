/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <fcntl.h>
#include <stdio.h>

#include "file.h"

FILE *fopenat(int fd, const char *path, int o, const char *m) {
    if ((fd = openat(fd, path, o)) == -1) {
        return NULL;
    }

    return fdopen(fd, m);
}

