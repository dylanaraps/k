#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "log.h"
#include "pkg.h"

struct version pkg_version(char *repo_dir) {
    struct version version = {0};
    FILE *file;
    char *buf = 0;

    chdir(repo_dir);
    file = fopen("version", "r");

    if (!file) {
        log_error("version file does not exist");
    }

    getline(&buf, &(size_t){0}, file);
    fclose(file);

    if (!buf) {
        log_error("version file does is invalid");
    }

    version.version = strtok(buf,    " 	\n");
    version.release = strtok(NULL,   " 	\n");

    if (!version.release) {
        log_error("release field empty");
    }

    chdir(PWD);
    return version;
}

