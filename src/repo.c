#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "repo.h"

char **repo_load(void) {
    char *path = getenv("KISS_PATH");

    if (!path || path[0] == '\0') {
        printf("error: KISS_PATH must be set\n");
        exit(1);
    }

    path = join_string(path, "/var/db/kiss/installed", ":");

    return split_string(path, ":");
}
