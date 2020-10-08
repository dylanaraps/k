#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "vec.h"
#include "str.h"
#include "repo.h"
#include "pkg.h"

static char **REPOS = NULL;

enum actions {
    ACTION_ALTERNATIVES,
    ACTION_BUILD,
    ACTION_CHECKSUM,
    ACTION_DOWNLOAD,
    ACTION_INSTALL,
    ACTION_LIST,
    ACTION_REMOVE,
    ACTION_SEARCH,
    ACTION_UPDATE,
};

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

int main (int argc, char *argv[]) {
    int action = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
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
        puts("version:      Package manager version");
        puts("\nRun 'kiss help-ext' to see all actions");
        return 0;

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

    } else if (strcmp(argv[1], "version") == 0 ||
               strcmp(argv[1], "v") == 0) {
        puts("0.0.1");
        return 0;

    } else {
        run_extension(argv);
        return 0;
    }

    REPOS = repo_init();
    package *PKGS = NULL;

    for (int i = 2; i < argc; i++) {
        vec_add(PKGS, pkg_init(argv[i]));
    }

    if (vec_size(PKGS) == 0) {
        switch (action) {
            case ACTION_BUILD:
            case ACTION_CHECKSUM:
            case ACTION_DOWNLOAD:
            case ACTION_INSTALL:
            case ACTION_REMOVE:
                // todo: use pwd as arg, prepend to KISS_PATH
                break;

            case ACTION_LIST: {
                PKGS = pkg_init_db();
                break;
            }
        }
    }

    switch (action) {
        case ACTION_LIST: {
            for (size_t i = 0; i < vec_size(PKGS); ++i) {
                if (!pkg_list(PKGS[i].name)) {
                    die("package '%s' not installed", PKGS[i].name);
                }

                puts(PKGS[i].name);
            }
            break;
        }

        case ACTION_SEARCH: {
            for (size_t i = 0; i < vec_size(PKGS); ++i) {
                char *match = repo_find(PKGS[i].name, 1, REPOS);

                if (match) {
                    free(match);
                } else {
                    die("no results for '%s'", PKGS[i].name);
                }
            }

            break;
        }
    }

    repo_free(REPOS);
    pkg_free(PKGS);
}
