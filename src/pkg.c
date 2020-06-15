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
    new_pkg->path = pkg_find(pkg_name, REPOS);
    new_pkg->version = pkg_version(pkg_name, *new_pkg->path);

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
    char *path = NULL;
    char **split;
    FILE *file;
    size_t  lsiz=0;
    char*   lbuf=0;
    ssize_t llen=0;
        
    path = join_string(repo, "version", "/");
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

char **pkg_find(char *pkg_name, char **repos) {
   char **paths = NULL;
   char *repo;
   int  n = 0;
   DIR  *d;
   struct dirent *dir;

   while (*repos != NULL) {
       d = opendir(*repos);

       if (d == NULL) {
           continue;
       }

       while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg_name, dir->d_name) == 0) {
                repo  = join_string(*repos, pkg_name, "/");
                paths = realloc(paths, sizeof(char*) * ++n);

                if (paths == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }

                paths[n - 1] = repo;
            }
       }

       ++repos;
       closedir(d);
   }

   paths = realloc(paths, sizeof(char*) * (n + 1));
   paths[n] = 0;

   if (*paths) {
       return paths;

   } else {
       printf("error: %s not in any repository\n", pkg_name);
       exit(1);
   }
}

void pkg_list(char *pkg_name) {
    struct version version;
    char *db = "/var/db/kiss/installed"; 
    char *path;
    DIR  *d;
    struct dirent *dir;

    d = opendir(db);

    if (d == NULL) {
        printf("error: No packages installed\n");
        exit(1);
    }

    while ((dir = readdir(d)) != NULL) {
        if (pkg_name != NULL && strcmp(pkg_name, dir->d_name) == 0) {
            path = join_string(db, pkg_name, "/");
            version = pkg_version(pkg_name, path);
            printf("%s %s %s\n", pkg_name, version.version, version.release);
            goto done;
        }
    }

done:
    closedir(d);
}
