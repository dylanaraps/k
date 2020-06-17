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
    FILE *f;
    char *buf = 0;

    SAVE_CWD;
    chdir(repo_dir);

    f = fopen("version", "r");

    if (!f) {
        log_error("version file does not exist");
    }

    // Only need the first line.
    getline(&buf, &(size_t){0}, f);
    fclose(f);

    if (!buf) {
        log_error("version file is invalid");
    }

    version.version = strtok(buf,    " 	\n");
    version.release = strtok(NULL,   " 	\n");

    if (!version.release) {
        log_error("release field empty");
    }

    LOAD_CWD;
    return version;
}

