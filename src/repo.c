#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* getenv, size_t */
#include <string.h> /* strdup, strlen, strtok */
#include <limits.h> /* PATH_MAX */
#include <stdio.h> /* printf */

#include "log.h"
#include "util.h"
#include "repo.h"

char **REPOS;

void repo_init(void) {
    char *kiss_path = strdup(getenv("KISS_PATH"));    
    char *tmp = 0;
    int repo_len = 0;
    int i;
    int j = 0;

    if (!kiss_path) {
        die("KISS_PATH must be set");
    }
    if (!strchr(kiss_path, '/')) {
        die("Invalid KISS_PATH");
    }
   
    /* Add +1 due to inbetween count */
    /* Add +1 for the fallback */
    repo_len = cntchr(kiss_path, ':') + 2;
    REPOS = xmalloc(repo_len * sizeof(char *));

    for (i = 0; i < repo_len; i++) {
        tmp = strtok(i ? NULL : kiss_path, ":");

        /* add fallback */
        if (!tmp) {
            tmp = "/var/db/kiss/installed";
        }

        if (tmp[0] != '/') {
            die("Repository must be absolute");
        }

        if (strlen(tmp) > PATH_MAX) {
            die("Repository exceeds PATH_MAX");
        }

        REPOS[i] = xmalloc(PATH_MAX);
        strcpy(REPOS[i], tmp);
    }

    free(kiss_path);

    for (i = 0; i < repo_len; i++) {
        printf("%s\n", REPOS[i]);
    }
}
