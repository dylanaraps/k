#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>

#include "cache.h"
#include "checksum.h"
#include "build.h"
#include "source.h"
#include "find.h"
#include "list.h"
#include "version.h"
#include "log.h"
#include "repo.h"
#include "util.h"
#include "vec.h"
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
    struct sigaction sa = {0};
    package *pkg = NULL;

    if (argc == 1) {
        usage();
    }

    repo_init();

    sa.sa_handler = exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    atexit(cache_destroy);

    for (int i = 2; i < argc; i++) {
        vec_push_back(pkg, pkg_init(argv[i]));
    }

    /* yucky global for atexit :( */
    PKG = vec_begin(pkg);

    switch (argv[1][0]) {
        case 'b':
            cache_init();
            pkg_iter(pkg, pkg_state_init);
            pkg_iter(pkg, pkg_source);
            pkg_iter(pkg, pkg_verify);
            pkg_iter(pkg, pkg_build);
            break;

        case 'c':
            cache_init();
            pkg_iter(pkg, pkg_state_init);
            pkg_iter(pkg, pkg_source);
            pkg_iter(pkg, pkg_checksums);
            pkg_iter(pkg, checksum_to_file);

            break;

        case 'd':
            cache_init();
            pkg_iter(pkg, pkg_state_init);
            pkg_iter(pkg, pkg_source);

            break;

        case 's':
            pkg_iter(pkg, pkg_paths);
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
