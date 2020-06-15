#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "pkg.h"

void pkg_load(package **head, char *pkg_name) {
    package *new_pkg = (package*) malloc(sizeof(package));    
    package *last = *head;

    if (new_pkg == NULL) {
        printf("error: Failed to allocate memory\n");
        exit(1);
    }

    new_pkg->next = NULL;
    new_pkg->name = pkg_name;
    new_pkg->repository = pkg_find(pkg_name, REPOS);
    new_pkg->path       = join_string(new_pkg->repository, pkg_name, "/");
    new_pkg->version    = pkg_version(pkg_name, new_pkg->repository);

    if (*head == NULL) {
        new_pkg->prev = NULL;
        *head = new_pkg;
        return;
    }

    while (last->next != NULL) {
        last = last->next;
    }

    last->next = new_pkg;
    new_pkg->prev = last;
}


struct version pkg_version(char *pkg_name, char *repo) {
    struct version version;
    char *path;
    char **split;
    FILE *file;
    size_t  lsiz=0;
    char*   lbuf=0;
    ssize_t llen=0;
        
    path = join_string(repo, pkg_name, "/");
    path = join_string(path, "version", "/");
    file = fopen(path, "r");

    if (file == NULL) {
        printf("error: version file does not exist\n");
        exit(1);
    }

    llen = getline(&lbuf, &lsiz, file);
    fclose(file);

    if (lbuf == NULL) {
        printf("error: version file is empty\n");
        exit(1);
    }

    if ((lbuf)[llen - 1] == '\n') {
        (lbuf)[llen - 1] = '\0';
    }

    split = split_string(lbuf, " ");

    if (split[0] == NULL) {
        printf("error: version file empty\n");
        exit(1);
    }

    if (split[1] == NULL) {
        printf("error: release field missing\n");
        exit(1);
    }

    version.version = split[0];
    version.release = split[1];

    return version;    
}

char *pkg_find(char *pkg_name, char **repos) {
   DIR  *d;
   struct dirent *dir;

   while (*repos != NULL) {
       d = opendir(*repos);

       if (d == NULL) {
           continue;
       }

       while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg_name, dir->d_name) == 0) {
                closedir(d);
                return *repos;
            }
       }

       ++repos;
       closedir(d);
   }

   printf("error: %s not in any repository\n", pkg_name);
   exit(1);
}
