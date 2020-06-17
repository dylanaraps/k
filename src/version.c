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

    SAVE_CWD;
    chdir(repo_dir);

    file = fopen("version", "r");

    if (!file) {
        log_error("version file does not exist");
    }

    // Only need the first line.
    // TODO: POSIX but unportable(?).
    getline(&buf, &(size_t){0}, file);

    if (!buf) {
        log_error("version file is invalid");
    }

    fclose(file);

    version.version = strtok(buf,  " 	\r\n");
    version.release = strtok(NULL, " 	\r\n");

    if (!version.release) {
        log_error("release field empty");
    }

    LOAD_CWD;
    return version;
}
