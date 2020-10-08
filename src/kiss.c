#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "util.h"
#include "vec.h"
#include "str.h"
#include "repo.h"
#include "pkg.h"

static package *pkgs = NULL;

enum actions {
    ACTION_ALTERNATIVES,
    ACTION_BUILD,
    ACTION_CHECKSUM,
    ACTION_DOWNLOAD,
    ACTION_EXTENSION,
    ACTION_INSTALL,
    ACTION_LIST,
    ACTION_REMOVE,
    ACTION_SEARCH,
    ACTION_UPDATE,
    ACTION_USAGE,
    ACTION_VERSION,
};

static void exit_handler(void) {
    repo_free();

    if (pkgs) {
        pkg_free(pkgs);
    }
}

static void run_extension(char *argv[]) {
    str cmd = {0};

    str_cat(&cmd, "kiss-");
    str_cat(&cmd, argv[1]);

    int err = execvp(cmd.buf, ++argv);

    str_free(&cmd);

    if (err == -1) {
        perror("execvp");
        exit(1);
    }
}

static int run_action(int action, char **argv, int argc) {
    for (int i = 2; i < argc; i++) {
        vec_add(pkgs, pkg_init(argv[i]));
    }

    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_REMOVE:
            if (argc < 3) {
                char *cwd = xgetcwd();
                int err = PATH_prepend(dirname(cwd), "KISS_PATH");
                free(cwd);

                if (err == 1) {
                    die("failed to prepend to KISS_PATH");
                }

                cwd = xgetcwd();
                vec_add(pkgs, pkg_init(basename(cwd)));
                free(cwd);
            }
            break;
    }

    switch (action) {
        case ACTION_LIST:
            pkg_list_all(pkgs);
            break;

        case ACTION_SEARCH:
            repo_find_all(pkgs);
            break;

        case ACTION_EXTENSION:
            run_extension(argv);
            break;

        case ACTION_VERSION:
            puts("0.0.1");
            break;

        default:
            puts("kiss [b|c|d|l|s|v] [pkg]...");
            puts("alternatives List and swap to alternatives");
            puts("build        Build a package");
            puts("checksum     Generate checksums");
            puts("download     Pre-download all sources");
            puts("install      Install a package");
            puts("list         List installed packages");
            puts("remove       Remove a package");
            puts("search       Search for a package");
            puts("update       Update the system");
            puts("version:     Package manager version");
            puts("\nRun 'kiss help-ext' to see all actions");
    }

    return 0;
}

int main (int argc, char *argv[]) {
    int action = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
        action = ACTION_USAGE;

    } else if (strcmp(argv[1], "alternatives") == 0 ||
               strcmp(argv[1], "a") == 0) {
        action = ACTION_ALTERNATIVES;

    } else if (strcmp(argv[1], "build") == 0 ||
               strcmp(argv[1], "b") == 0) {
        action = ACTION_BUILD;

    } else if (strcmp(argv[1], "checksum") == 0 ||
               strcmp(argv[1], "c") == 0) {
        action = ACTION_CHECKSUM;

    } else if (strcmp(argv[1], "download") == 0 ||
               strcmp(argv[1], "d") == 0) {
        action = ACTION_DOWNLOAD;

    } else if (strcmp(argv[1], "install") == 0 ||
               strcmp(argv[1], "i") == 0) {
        action = ACTION_INSTALL;

    } else if (strcmp(argv[1], "list") == 0 ||
               strcmp(argv[1], "l") == 0) {
        action = ACTION_LIST;

    } else if (strcmp(argv[1], "remove") == 0 ||
               strcmp(argv[1], "r") == 0) {
        action = ACTION_REMOVE;

    } else if (strcmp(argv[1], "search") == 0 ||
               strcmp(argv[1], "s") == 0) {
        action = ACTION_SEARCH;

    } else if (strcmp(argv[1], "update") == 0 ||
               strcmp(argv[1], "u") == 0) {
        action = ACTION_UPDATE;

    } else if (strcmp(argv[1], "version") == 0 ||
               strcmp(argv[1], "v") == 0) {
        action = ACTION_VERSION;

    } else {
        action = ACTION_EXTENSION;
    }

    atexit(exit_handler);

    return run_action(action, argv, argc);
}
