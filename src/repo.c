#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "repo.h"

char **repo_load(void) {
    char *path = getenv("KISS_PATH");
    char *tok;
    char **res = NULL;
    int n = 0;

    if (!path || path[0] == '\0') {
        printf("error: KISS_PATH must be set\n");
        exit(1);
    }

    tok = strtok(path, ":"); 

    while (tok) {
        res = realloc(res, sizeof(char*) * ++n);
        
        if (res == NULL) {
            printf("Failed to allocate memory\n");
            exit(1);
        }

        res[n - 1] = tok;
        tok = strtok(NULL, ":");
    }

    res = realloc (res, sizeof(char*) * ++n);
    res[n - 1] = "/var/db/kiss/installed";
    res = realloc (res, sizeof(char*) * (n + 1));
    res[n] = 0;

    return res;
}
