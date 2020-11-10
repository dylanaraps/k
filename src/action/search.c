/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <unistd.h>
#include <string.h>

#include "buf.h"

int action_search(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    for (char *t = strtok(0, ":"); t; t = strtok(NULL, ":")) {

    }

    return 0;
}

