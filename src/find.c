#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "log.h"
#include "pkg.h"

void pkg_find(package *pkg) {
   char *pwd;
   char cwd[PATH_MAX];
   int i;

   pkg->path_len = 0;
   pkg->path = (char **) malloc(sizeof(char*) * 1);

   for (i =0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
           log_fatal("Repository %s not accessible", REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
           pwd = getcwd(cwd, sizeof(cwd));
           pkg->path[pkg->path_len] = malloc(sizeof(char) * ((strlen(pwd) + 1)));

           if (pkg->path[pkg->path_len] == NULL) {
               log_fatal("Failed to allocate memory");
           }

           strcpy(pkg->path[++pkg->path_len - 1], pwd);
       }
   }

   chdir(PWD);
   pkg->path[pkg->path_len] = 0;

   if (pkg->path_len == 0) {
       log_fatal("%s not in any repository", pkg->name);
   }
}
