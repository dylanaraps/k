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

static char **repos = NULL;
static package *pkgs = NULL;

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

static void exit_handler(void) {
    if (repos) {
        repo_free(repos);
    }

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

static int run_action(int action) {
    if (vec_size(pkgs) == 0) {
        switch (action) {
            case ACTION_BUILD:
            case ACTION_CHECKSUM:
            case ACTION_DOWNLOAD:
            case ACTION_INSTALL:
            case ACTION_REMOVE:
            case ACTION_SEARCH:
                break;

            case ACTION_LIST: {
                pkgs = pkg_init_db();
                break;
            }
        }
    }

    switch (action) {
        case ACTION_LIST: {
            for (size_t i = 0; i < vec_size(pkgs); ++i) {
                if (!pkg_list(pkgs[i].name)) {
                    msg("package '%s' not installed", pkgs[i].name);
                    return 1;
                }

                puts(pkgs[i].name);
            }
            break;
        }

        case ACTION_SEARCH: {
            for (size_t i = 0; i < vec_size(pkgs); ++i) {
                char *match = repo_find(pkgs[i].name, 1, repos);

                if (match) {
                    free(match);
                } else {
                    msg("no results for '%s'", pkgs[i].name);
                    return 1;
                }
            }

            break;
        }
    }

    return 0;
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

    } else {
        run_extension(argv);
    }

    if (!action) {
        return 0;
    }

    repos = repo_init();
    atexit(exit_handler);

    for (int i = 2; i < argc; i++) {
        vec_add(pkgs, pkg_init(argv[i]));
    }

    return run_action(action);
}
