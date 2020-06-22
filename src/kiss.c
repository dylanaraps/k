#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <limits.h> /* PATH_MAX */
#include <signal.h> /* */

#include "cache.h"
#include "checksum.h"
#include "depends.h"
#include "build.h"
#include "signal.h"
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
    package *pkg;

    if (argc == 1) {
        usage();
    }

    sig_init();
    repo_init();

    for (int i = 2; i < argc; i++) {
        pkg_init(&pkg, argv[i], 0);
    }
    package *tmp;

    switch (argv[1][0]) {
        case 'b':
            cache_init();
            pkg_iter(pkg, pkg_state_init, NULL);
            pkg_iter(pkg, pkg_depends, "Checking sources");
            pkg_iter(pkg, resolve_dep, "Checking sources");

            for (tmp = pkg; tmp; tmp = tmp->next) {
                printf("%s\n", tmp->name);
            }
            exit(0);
            pkg_iter(pkg, pkg_source,  "Checking sources");
            pkg_iter(pkg, pkg_verify,  "Verifying checksums");
            pkg_iter(pkg, pkg_build,   "Building packages");
            break;

        case 'c':
            cache_init();
            pkg_iter(pkg, pkg_state_init, NULL);
            pkg_iter(pkg, pkg_source,       "Checking sources");
            pkg_iter(pkg, pkg_checksums,    "Generating checksums");
            pkg_iter(pkg, checksum_to_file, "Saving checksums");

            break;

        case 'd':
            cache_init();
            pkg_iter(pkg, pkg_state_init, NULL);
            pkg_iter(pkg, pkg_source, "Downloading sources");

            break;

        case 's':
            pkg_iter(pkg, pkg_paths, NULL);
            break;

        case 'l':
            pkg_list_all(pkg);
            break;

        case 'v':
            printf("0.0.1\n");
            break;

        default:
            usage();
    }
}
