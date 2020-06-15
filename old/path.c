#include <dirent.h> 
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "path.h"
#include "kiss.h"

char **path_load(void) {
    char *path = getenv("KISS_PATH");

    if (!path || path[0] == '\0') {
        printf("KISS_PATH must be set.\n");
        exit(1);
    }

    path = strjoin(path, "/var/db/kiss/installed", ":");

    return str_to_array(path, ":");
}

char *path_find(char *pkg, int all) {
    char **repo = KISS_PATH;
    char *match;
    DIR  *d;
    struct dirent *dir;

    while (*repo != 0) {
        if (!(d = opendir(*repo))) continue;

        while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg, dir->d_name) != 0) continue;

            match = strjoin(*repo, dir->d_name, "/");

            if (all) {
                printf("%s\n", match);
                break;
            } else {
                closedir(d);
                return match;
            }
        }

        ++repo;
        closedir(d);
    }

    printf("%s (%s)\n", pkg, "Package not in any repository");

    if (!all)
        exit(1);

    return NULL;
}
