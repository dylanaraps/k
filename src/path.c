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
    char *path, *repo, **res;
    int  n = 0;

    path = getenv("KISS_PATH");

    if (!path || path[0] == '\0')
        exit(1);

    res  = NULL;
    repo = strtok(path, ":"); 

    while (repo) {
        res = realloc(res, sizeof(char*) * ++n);
        
        if (res == NULL)
            exit(1);

        res[n - 1] = repo;
        repo = strtok(NULL, ":");
    }

    res = realloc (res, sizeof(char*) * (n + 1));
    res[n] = 0;

    return res;
}

char *path_find(char *pkg) {
    char **repo = KISS_PATH;
    DIR  *d;
    struct dirent *dir;

    while (*repo != 0) {
        if (!(d = opendir(*repo))) continue;

        while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg, dir->d_name) != 0) continue;

            closedir(d);
            return strjoin(*repo, dir->d_name, "/");
        }

        ++repo;
        closedir(d);
    }

    return *repo;
}
