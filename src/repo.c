#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "repo.h"

char **repo_load(void) {
    char *path = strdup(getenv("KISS_PATH"));
    char *tok;
    char **res = NULL;
    int n = 0;

    if (!path || path[0] == '\0') {
        printf("error: KISS_PATH must be set\n");
        exit(1);
    }

    // 24 is below string + ":".
    tok = strtok(path, ":"); 
    res = calloc(strlen(path) + 24, sizeof(char *));

    if (res == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }

    while (tok) {
        res[++n - 1] = tok;
        tok = strtok(NULL, ":");
    }
    res[n] = "/var/db/kiss/installed";

    return res;
}
