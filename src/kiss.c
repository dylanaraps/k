#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <curl/curl.h>

#include "checksum.h"
#include "source.h"
#include "repo.h"
#include "list.h"
#include "pkg.h"

char **REPOS = NULL;
char PWD[PATH_MAX];
const char *HOME;
const char *XDG_CACHE_HOME;
char *CAC_DIR, *MAK_DIR, *PKG_DIR, *TAR_DIR, *SRC_DIR, *LOG_DIR, *BIN_DIR;

void args(int argc, char *argv[]) {
    package *head = NULL;
    package *root;

    if (argc == 1) {
        printf("kiss [a|b|c|i|l|r|s|u|v] [pkg]...\n");
        printf("alternatives: List and swap to alternatives\n");
        printf("build:        Build a package\n");
        printf("checksum:     Generate checksums\n");
        printf("download:     Pre-download all sources\n");
        printf("install:      Install a package\n");
        printf("list:         List installed packages\n");
        printf("remove:       Remove a package\n");
        printf("search:       Search for a package\n");
        printf("update:       Check for updates\n");
        printf("version:      Package manager version\n");

        exit(0);
    }

    for (int i = 2; i < argc; i++) {
        pkg_load(&head, argv[i]);
    }
    root = head;

    if (!strcmp(argv[1], "c") || !strcmp(argv[1], "checksum")) {
        curl_global_init(CURL_GLOBAL_ALL);
        while (head) {
            pkg_sources(head);
            pkg_checksums(head);
            head = head->next;
        }

        while (root) {
            printf("%s\n", root->name);
            printf("-------------------\n");

            printf("Sources:\n");
            for (int i = 0; i < root->src_len; i++) {
                printf("%d -> %s\n", i, root->srcs[i]);
            }
            printf("\nSums:\n");
            for (int i = 0; i < root->sum_len; i++) {
                printf("%d -> %s\n", i, root->sums[i]);
            }

            printf("Paths:\n");
            for (int i = 0; i < root->path_len; i++) {
                printf("%d -> %s\n", i, root->path[i]);
            }

            root = root->next;
        }


    } else if (!strcmp(argv[1], "d") || !strcmp(argv[1], "download")) {
        curl_global_init(CURL_GLOBAL_ALL);
        while (head) {
            pkg_sources(head);
            head = head->next;
        }

    } else if (!strcmp(argv[1], "l") || !strcmp(argv[1], "list")) {
        if (argc == 2) {
           pkg_list_all(); 

        } else {
            while (head) {
                pkg_list(head->name);
                head = head->next;
            }
        }

    } else if (!strcmp(argv[1], "s") || !strcmp(argv[1], "search")) {
        while (head) {
            for (char *c = *head->path; c; c=*++head->path) {
                printf("%s\n", *head->path);
            }

            head = head->next;
        }

    } else if (!strcmp(argv[1], "v") || !strcmp(argv[1], "version")) {
        printf("0.0.1\n");
    }

    free(head);
}

int main (int argc, char *argv[]) {
    getcwd(PWD, sizeof(PWD));
    cache_init();
    REPOS = repo_load();
    args(argc, argv);
    return 0;
}
