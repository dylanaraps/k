#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "repo.h"
#include "log.h"
#include "util.h"
#include "pkg.h"

char **repo_load(void) {
    char *path;
    char *tok;
    char **res = {0};
    int n = 0;

    path = strdup(getenv("KISS_PATH"));

    if (!path || path[0] == '\0')
        die("KISS_PATH must be set");

    res = xmalloc(strlen(path) + 24);
    tok = strtok(path, ":"); 

    while (tok) {
        res[n++] = tok;
        tok = strtok(NULL, ":");
    }

    res[n] = "/var/db/kiss/installed";
    REPO_LEN = n + 1;

    // Must be free()'d by caller.
    return res;
}
