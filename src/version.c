#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "pkg.h"

struct version pkg_version(char *repo_dir) {
    struct version version = {0};
    FILE *file;
    char *buf = 0;

    chdir(repo_dir);
    file = fopen("version", "r");

    if (!file) {
        printf("error: version file does not exist\n");
        exit(1);
    }

    getline(&buf, &(size_t){0}, file);
    fclose(file);

    if (!buf) {
        printf("error: version file is incorrect\n");
        exit(1);
    }

    version.version = strtok(buf,    " 	\n");
    version.release = strtok(NULL,   " 	\n");

    if (!version.release) {
        printf("error: release field missing\n");
        exit(1);
    }

    chdir(PWD);
    return version;
}

