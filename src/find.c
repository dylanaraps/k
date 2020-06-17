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
   char pwd_buf[PATH_MAX + 1];
   int i;

   SAVE_CWD;
   pkg->path_len = 0;
   pkg->path = malloc(sizeof(char*) * 1);

   for (i = 0; i < REPO_LEN; i++) {
       if (chdir(REPOS[i]) != 0) {
           log_error("Repository %s not accessible", REPOS[i]);
       }

       if (chdir(pkg->name) == 0) {
           pwd = getcwd(pwd_buf, sizeof(pwd_buf));
           pkg->path[pkg->path_len] = malloc(sizeof(char) * ((strlen(pwd) + 1)));

           if (pkg->path[pkg->path_len] == NULL) {
               log_error("Failed to allocate memory");
           }

           strcpy(pkg->path[++pkg->path_len - 1], pwd);
       }
   }

   LOAD_CWD;
   pkg->path[pkg->path_len] = 0;

   if (pkg->path_len == 0) {
       PKG = pkg->name;
       log_error("not in any repository");
   }
}
