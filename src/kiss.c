#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "build.h"
#include "checksum.h"
#include "extract.h"
#include "source.h"
#include "repo.h"
#include "list.h"
#include "cache.h"
#include "pkg.h"

char *OLD_CWD = NULL;
char **REPOS = NULL;
char *PKG = NULL;
int  REPO_LEN = 0;
char HOME[PATH_MAX];
char XDG_CACHE_HOME[PATH_MAX];
char CAC_DIR[PATH_MAX];
char MAK_DIR[PATH_MAX];
char PKG_DIR[PATH_MAX];
char TAR_DIR[PATH_MAX];
char SRC_DIR[PATH_MAX];
char BIN_DIR[PATH_MAX];
char *OLD_CWD;
char old_cwd_buf[PATH_MAX];

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
    package *head = NULL;

    if (argc == 1)
        usage();

    cache_init();
    atexit(cache_destroy);
    REPOS = repo_load();

    for (int i = 2; i < argc; i++) {
        pkg_init(&head, argv[i]);
        PKG = head->name;
    }

    switch (argv[1][0]) {
    case 'b':
        for (package *tmp = head; tmp; tmp = tmp->next)
            pkg_sources(tmp);

        for (package *tmp = head; tmp; tmp = tmp->next)
            pkg_verify(tmp);

        for (package *tmp = head; tmp; tmp = tmp->next) {
            pkg_extract(tmp);
            pkg_build(tmp);
        }

        break;

    case 'c':
        for (package *tmp = head; tmp; tmp = tmp->next)
            pkg_sources(tmp);

        for (package *tmp = head; tmp; tmp = tmp->next)
            pkg_checksums(tmp);

        break;

    case 'd':
        for (package *tmp = head; tmp; tmp = tmp->next)
            pkg_sources(tmp);

        break;

    case 'l':
        if (argc == 2) {
           pkg_list_all(head); 

        } else {
            for (package *tmp = head; tmp; tmp = tmp->next)
                pkg_list(tmp);
        }
        break;

    case 's':
        for (package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;

            for (int i = 0; i < tmp->path_len; i++) {
                printf("%s\n", tmp->path[i]);
            }
        }
        break;

    case 'v':
        printf("0.0.1\n");
        break;

    default:
        usage();
    }

    pkg_destroy(head);
    return 0;
}
