#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "util.h"
#include "pkg.h"

void pkg_load(package **head, char *pkg_name) {
    package *new_pkg = (package*) malloc(sizeof(package));    
    package *last = *head;

    if (!new_pkg) {
        printf("error: Failed to allocate memory\n");
        exit(1);
    }

    new_pkg->next = NULL;
    new_pkg->name = pkg_name;
    new_pkg->path = pkg_find(pkg_name, REPOS);

    if (!*head) {
        new_pkg->prev = NULL;
        *head = new_pkg;
        return;
    }

    while (last->next) {
        last = last->next;
    }

    last->next = new_pkg;
    new_pkg->prev = last;
}


struct version pkg_version(char *repo_dir) {
    char **split;
    FILE *file;
    char*   lbuf=0;
    ssize_t llen=0;

    chdir(repo_dir);
    file = fopen("version", "r");

    if (!file) {
        printf("error: version file does not exist\n");
        exit(1);
    }

    llen = getline(&lbuf, &(size_t){0}, file);
    fclose(file);

    if (!lbuf) {
        printf("error: version file is empty\n");
        exit(1);
    }

    if ((lbuf)[llen - 1] == '\n') {
        (lbuf)[llen - 1] = '\0';
    }

    split = split_string(lbuf, " ");

    if (!split[0]) {
        printf("error: version file empty\n");
        exit(1);
    }

    if (!split[1]) {
        printf("error: release field missing\n");
        exit(1);
    }

    chdir(PWD);

    return (struct version) {
        .version = split[0],
        .release = split[1],
    };
}

char **pkg_find(char *pkg_name, char **repos) {
   char **paths = NULL;
   int  n = 0;
   DIR  *d;
   struct dirent *dir;
   char cwd[PATH_MAX];

   while (*repos) {
       d = opendir(*repos);
       chdir(*repos);

       if (!d) {
           continue;
       }

       while ((dir = readdir(d)) != NULL) {
            if (strcmp(pkg_name, dir->d_name) == 0) {
                chdir(pkg_name);
                paths = realloc(paths, sizeof(char*) * ++n);

                if (paths == NULL) {
                    printf("Failed to allocate memory\n");
                    exit(1);
                }

                paths[n - 1] = getcwd(cwd, sizeof(cwd));
                chdir("..");
            }
       }

       ++repos;
       closedir(d);
   }

   chdir(PWD);
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

    if (!d) {
        printf("error: No packages installed\n");
        exit(1);
    }

    while ((dir = readdir(d))) {
        if (pkg_name != NULL && strcmp(pkg_name, dir->d_name) == 0) {
            path = join_string(db, pkg_name, "/");
            version = pkg_version(path);
            printf("%s %s %s\n", pkg_name, version.version, version.release);
            goto done;
        }
    }

done:
    closedir(d);
}
