#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "repo.h"
#include "pkg.h"

char **REPOS = NULL;

void args(int argc, char *argv[]) {
    package *head = NULL;

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

    if (!strcmp(argv[1], "d") || !strcmp(argv[1], "download")) {
        //

    } else if (!strcmp(argv[1], "l") || !strcmp(argv[1], "list")) {
        while (head) {
            pkg_list(head->name);
            head = head->next;
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
    REPOS = repo_load();
    args(argc, argv);
    return 0;
}
