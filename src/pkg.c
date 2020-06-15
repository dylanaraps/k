#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "util.h"
#include "pkg.h"

void pkg_load(package **head, char *pkg_name) {
    package *new_pkg = (package*) malloc(sizeof(package));    
    package *last = *head;

    if (new_pkg == NULL) {
        printf("error: Failed to allocate memory\n");
        exit(1);
    }

    new_pkg->name = pkg_name;
    new_pkg->next = NULL;
    new_pkg->repository = pkg_find(pkg_name, REPOS);

    if (new_pkg->repository == NULL) {
        printf("error: %s not in any repository\n", pkg_name);
        exit(1);
    }

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

   return NULL; 
}
