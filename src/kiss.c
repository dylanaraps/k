#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <limits.h> /* PATH_MAX */

#include "log.h"
#include "repo.h"
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
    package *pkgs = NULL;

    if (argc == 1) {
        usage();
    }

    repo_init();
    atexit(repo_destroy);

    for (int i = 2; i < argc; i++) {
        pkg_init(&pkgs, argv[i]);
    }

    switch (argv[1][0]) {
        case 's':
            break;

        case 'v':
            printf("0.0.1\n");
            break;
    }

    free(pkgs);
}
