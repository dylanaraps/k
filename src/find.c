#include <dirent.h> 
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "find.h"

char *path_query(const char *pkg) {
    char *path = getenv("KISS_PATH");
    char *repo = strtok(path, ":"); 
    DIR  *d;
    struct dirent *dir;

    if (pkg[0] == '\0') return NULL;

    while (repo != NULL) {
        if (!(d = opendir(repo))) continue;

        while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg, dir->d_name) != 0) continue;

            char *match = malloc(strlen(repo) + strlen(dir->d_name) + 2);
            strcpy(match, repo);
            strcat(match, "/");
            strcat(match, dir->d_name);
            closedir(d);

            // Remember to free this later.
            return match;
        }

        closedir(d);
        repo = strtok(NULL, ":");
    }

    return NULL;
}
