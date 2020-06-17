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

    if (!(f = fopen("version", "r"))) {
        log_error("version file does not exist");
    }

    // Only need the first line.
    if (!getline(&buf, &(size_t){0}, f)) {
        log_error("version file is invalid");
    }

    fclose(f);

    version.version = strtok(buf,  " 	\r\n");
    version.release = strtok(NULL, " 	\r\n");

    if (!version.release) {
        log_error("release field empty");
    }

    LOAD_CWD;
    return version;
}
