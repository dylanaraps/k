/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "action.h"
#include "buf.h"
#include "cache.h"
#include "error.h"

static int (*actions[])(buf **, int, char *[]) = {
    ['a'] = NULL,
    ['b'] = NULL,
    ['c'] = NULL,
    ['d'] = NULL,
    ['i'] = NULL,
    ['l'] = action_list,
    ['r'] = NULL,
    ['s'] = action_search,
    ['u'] = NULL,
};

static void version(char *arg0) {
    printf("%s 0.0.1 (compiled %s)\n", arg0, __DATE__);
}

static void usage(char *arg0) {
    fputs(arg0, stdout);
    fputs(" [a|b|c|d|i|l|r|s|u|v] [pkg]...\n", stdout);

    puts("alt          List and swap to alternatives");
    puts("build        Build a package");
    puts("checksum     Generate checksums");
    puts("download     Pre-download all sources");
    puts("install      Install a package");
    puts("list         List installed packages");
    puts("remove       Remove a package");
    puts("search       Search for a package");
    puts("update       Update the system");
    puts("version      Package manager version");
}

static int run_extension(char *argv[]) {
    char ext[256] = "kiss-";

    strncat(ext, *argv, 255);
    execvp(ext, argv);

    err("failed to execute extension %s", ext);
    return -1;
}

int main (int argc, char *argv[]) {
    int err = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
        usage(argv[0]);

// Check if argument matches an action. True for b==build and build==build
// strcmp is only reached when both first characters match.
#define ARG(a, b) ((*a) == (*b) && ((!a[1]) || strcmp(a, b) == 0))

    } else if (ARG(argv[1], "build")    ||
               ARG(argv[1], "checksum") ||
               ARG(argv[1], "download")) {
        struct cache c;

        if (cache_init(&c) < 0) {
            err = -1;
        }

        cache_free(&c);

    } else if (ARG(argv[1], "list") ||
               ARG(argv[1], "search")) {
        buf *buf = buf_alloc(0, 1024);

        if (!buf) {
            err("failed to allocate memory");
            return -ENOMEM;
        }

        err = actions[(unsigned char) argv[1][0]](&buf, argc, argv);

        buf_free(&buf);

    } else if (ARG(argv[1], "version")) {
        version(argv[0]);

    } else {
        err = run_extension(argv + 1);
    }

    return err;
}

