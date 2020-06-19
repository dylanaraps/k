#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* getenv, size_t */
#include <string.h> /* strdup, strlen, strtok */
#include <limits.h> /* PATH_MAX */
#include <stdio.h>  /* printf */
#include <unistd.h> /* chdir */

#include "log.h"
#include "util.h"
#include "repo.h"

char **REPOS = {0};
int REPO_LEN = 0;

void repo_init(void) {
    char *kiss_path = strdup(getenv("KISS_PATH"));    
    char *tmp = 0;
    int i;

    if (!kiss_path) {
        die("KISS_PATH must be set");
    }

    if (!strchr(kiss_path, '/')) {
        die("Invalid KISS_PATH");
    }
   
    /* Add +1 due to inbetween count */
    /* Add +1 for the fallback */
    REPO_LEN = 2 + cntchr(kiss_path, ':');
    REPOS = xmalloc(REPO_LEN * sizeof(char *));

    for (i = 0; i < REPO_LEN; i++) {
        tmp = strtok(i ? NULL : kiss_path, ":");

        /* add fallback */
        if (!tmp) {
            tmp = "/var/db/kiss/installed";

            /* todo: prepend KISS_ROOT */
        }

        if (tmp[0] != '/') {
            die("Repository must be absolute");
        }

        if (strlen(tmp) > PATH_MAX) {
            die("Repository exceeds PATH_MAX");
        }

        if (chdir(tmp) != 0) {
            die("Repository is not accessible");
        }

        REPOS[i] = xmalloc(PATH_MAX);
        strncpy(REPOS[i], tmp, PATH_MAX);
    }

    free(kiss_path);
}

void repo_destroy(void) {
    int i; 

    for (i = 0; i < REPO_LEN; i++) {
        free(REPOS[i]);
    }

    free(REPOS);
}
