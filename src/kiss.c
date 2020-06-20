#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <limits.h> /* PATH_MAX */

#include "cache.h"
#include "checksum.h"
#include "source.h"
#include "find.h"
#include "list.h"
#include "version.h"
#include "log.h"
#include "repo.h"
#include "util.h"
#include "pkg.h"

static void usage(void) {
    printf("kiss [b|c|d|l|s|v] [pkg]...\n");
    printf("build:        Build a package\n");
    printf("checksum:     Generate checksums\n");
    printf("download:     Pre-download all sources\n");
    printf("list:         List installed packages\n");
    printf("search:       Search for a package\n");
    printf("version:      Package manager version\n");

    exit(0);
}

int main (int argc, char *argv[]) {
    package *tmp;

    if (argc == 1) {
        usage();
    }

    cache_init();
    atexit(cache_destroy);
    repo_init();
    atexit(repo_destroy);

    for (int i = 2; i < argc; i++) {
        pkg_init(&PKG, argv[i]);
    }
    atexit(pkg_destroy_all);

    switch (argv[1][0]) {
        case 'c':
            printf("\n\033[1mCHECKING SOURCES\033[m\n");
            LINE;

            for (tmp = PKG; tmp; tmp = tmp->next) {
                printf("\033[1m%s\033[m\n", tmp->name);
                LINE;
                pkg_source(PKG);
                printf("\n");
            }

            printf("\n\033[1mGENERATING CHECKSUMS\033[m\n");
            LINE;

            for (tmp = PKG; tmp; tmp = tmp->next) {
                printf("\033[1m%s\033[m\n", tmp->name);
                LINE;
                pkg_checksums(PKG);
                printf("\n");
            }
            break;

        case 'd':
            for (tmp = PKG; tmp; tmp = tmp->next) {
                printf("\033[1m%s\033[m\n", tmp->name);
                LINE;
                pkg_source(PKG);
                printf("\n");
            }
            break;

        case 's':
            for (tmp = PKG; tmp; tmp = tmp->next) {
                for (int i = 0; i < PKG->path_l; i++) {
                    printf("%s\n", PKG->path[i]);
                }
            }
            break;

        case 'l':
            if (argc == 2) {
               pkg_list_all(PKG);

            } else {
                for (tmp = PKG; tmp; tmp = tmp->next) {
                    pkg_list(PKG);
                }
            }
            break;

        case 'v':
            printf("0.0.1\n");
            break;

        default:
            usage();
    }
}
