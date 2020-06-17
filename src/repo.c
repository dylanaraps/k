#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "repo.h"
#include "log.h"
#include "pkg.h"

char **repo_load(void) {
    char *path;
    char *tok;
    char **res = {0};
    int n = 0;

    path = strdup(getenv("KISS_PATH"));

    if (!path || path[0] == '\0') {
        log_error("KISS_PATH must be set");
    }

    res = malloc(strlen(path) + 24);

    if (!res) {
        log_error("Failed to allocate memory");
    }

    tok = strtok(path, ":"); 

    while (tok) {
        res[n++] = tok;
        tok = strtok(NULL, ":");
    }

    res[n] = "/var/db/kiss/installed";
    REPO_LEN = n + 1;

    return res;
}
