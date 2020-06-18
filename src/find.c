#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "log.h"
#include "util.h"
#include "pkg.h"

void pkg_find(package *pkg) {
   SAVE_CWD;

   pkg->path_len = 0;
   pkg->path = xmalloc(REPO_LEN * sizeof(char *));

   for (int i = 0; i < REPO_LEN; i++) {
       xchdir(REPOS[i]);

       if (chdir(pkg->name) == 0) {
           pkg->path[pkg->path_len] = xmalloc(PATH_MAX + 1);
           snprintf(pkg->path[pkg->path_len], PATH_MAX + 1, "%s/%s", 
                    REPOS[i], pkg->name);

           pkg->path_len++;
       }
   }

   LOAD_CWD;
   pkg->path[pkg->path_len] = 0;

   if (pkg->path_len == 0) {
       PKG = pkg->name;
       log_error("not in any repository");
   }
}
