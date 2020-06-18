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
   int i;
   int p_len;

   SAVE_CWD;
   pkg->path_len = 0;
   pkg->path = xmalloc(sizeof(char *) * REPO_LEN);

   for (i = 0; i < REPO_LEN; i++) {
       xchdir(REPOS[i]);

       if (chdir(pkg->name) == 0) {
           p_len = strlen(REPOS[i]) + strlen(pkg->name) + 2;
           pkg->path[pkg->path_len] = xmalloc(p_len);

           snprintf(pkg->path[pkg->path_len], p_len, "%s/%s", 
                    REPOS[i], pkg->name);

           ++pkg->path_len;
       }
   }

   LOAD_CWD;
   pkg->path[pkg->path_len] = 0;

   if (pkg->path_len == 0) {
       PKG = pkg->name;
       log_error("not in any repository");
   }
}
