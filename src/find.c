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
   char *pwd;
   char pwd_buf[PATH_MAX + 1];
   int i;

   SAVE_CWD;
   pkg->path_len = 0;
   pkg->path = xmalloc(sizeof(char*) * 1);

   for (i = 0; i < REPO_LEN; i++) {
       xchdir(REPOS[i]);
       xchdir(pkg->name);

       pwd = getcwd(pwd_buf, sizeof(pwd_buf));
       pkg->path[pkg->path_len] = xmalloc(strlen(pwd) + 1);
       strcpy(pkg->path[++pkg->path_len - 1], pwd);
   }

   LOAD_CWD;
   pkg->path[pkg->path_len] = 0;
   PKG = pkg->name;

   if (pkg->path_len == 0)
       log_error("not in any repository");
}
