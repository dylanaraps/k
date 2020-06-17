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
#include "pkg.h"

char *OLD_CWD = NULL;
char **REPOS = NULL;
char *PKG = NULL;
int  REPO_LEN = 0;
const char *HOME;
const char *XDG_CACHE_HOME;
char *CAC_DIR, *MAK_DIR, *PKG_DIR, *TAR_DIR, *SRC_DIR, *LOG_DIR, *BIN_DIR;
char *OLD_PWD;
char old_cwd_buf[PATH_MAX+1];

int main (int argc, char *argv[]) {
    cache_init();
    atexit(cache_destroy);
    REPOS = repo_load();
    package *head = NULL;

    if (argc == 1) {
        printf("kiss [b|c|d|l|s|v] [pkg]...\n");
        printf("build:        Build a package\n");
        printf("checksum:     Generate checksums\n");
        printf("download:     Pre-download all sources\n");
        printf("list:         List installed packages\n");
        printf("search:       Search for a package\n");
        printf("version:      Package manager version\n");

        exit(0);
    }

    for (int i = 2; i < argc; i++) {
        pkg_load(&head, argv[i]);
        PKG = head->name;
    }

    if (!strcmp(argv[1], "b") || !strcmp(argv[1], "build")) {
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_sources(tmp);
        }
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_verify(tmp);
        }
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_extract(tmp);
            pkg_build(tmp);
        }

    } else if (!strcmp(argv[1], "c") || !strcmp(argv[1], "checksum")) {
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_sources(tmp);
        }
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_checksums(tmp);
            checksum_to_file(tmp);
        }

    } else if (!strcmp(argv[1], "d") || !strcmp(argv[1], "download")) {
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;
            pkg_sources(tmp);
        }

    } else if (!strcmp(argv[1], "l") || !strcmp(argv[1], "list")) {
        if (argc == 2) {
           pkg_list_all(); 

        } else {
            for(package *tmp = head; tmp; tmp = tmp->next) {
                PKG = tmp->name;
                pkg_list(tmp->name);
            }
        }

    } else if (!strcmp(argv[1], "s") || !strcmp(argv[1], "search")) {
        for(package *tmp = head; tmp; tmp = tmp->next) {
            PKG = tmp->name;

            for (char *c = *tmp->path; c; c=*++tmp->path) {
                printf("%s\n", *tmp->path);
            }
        }

    } else if (!strcmp(argv[1], "v") || !strcmp(argv[1], "version")) {
        printf("0.0.1\n");
    }

    free(head);
    return 0;
}
