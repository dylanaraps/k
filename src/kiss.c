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
            pkg_iter(PKG, pkg_source, "Checking sources");
            pkg_iter(PKG, pkg_checksums, "Generating checksums");

            break;

        case 'd':
            pkg_iter(PKG, pkg_source, "Downloading sources");

            break;

        case 's':
            pkg_iter(PKG, pkg_paths, NULL);
            break;

        case 'l':
            if (argc == 2) {
                pkg_list_all(PKG);

            } else {
                pkg_iter(PKG, pkg_list, NULL);
            }
            break;

        case 'v':
            printf("0.0.1\n");
            break;

        default:
            usage();
    }
}
