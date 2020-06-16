#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "pkg.h"

void pkg_find(package *pkg) {
   char **repos = REPOS;
   char *pwd;
   char cwd[PATH_MAX];

   pkg->path_len = 0;
   pkg->path = (char **) malloc(sizeof(char*) * 1);

   while (*repos) {
       if (chdir(*repos) != 0) {
           printf("error: Repository not accessible\n");       
           exit(1);
       }

       if (chdir(pkg->name) == 0) {
           pwd = getcwd(cwd, sizeof(cwd));
           pkg->path[pkg->path_len] = malloc(sizeof(char) * ((strlen(pwd) + 1)));

           if (pkg->path[pkg->path_len] == NULL) {
               printf("Failed to allocate memory\n");
               exit(1);
           }

           strcpy(pkg->path[++pkg->path_len - 1], pwd);
       }

       ++repos;
   }

   chdir(PWD);
   pkg->path[pkg->path_len] = 0;

   if (pkg->path_len == 0) {
       printf("error: %s not in any repository\n", pkg->name);
       exit(1);
   }
}
