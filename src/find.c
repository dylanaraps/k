#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "pkg.h"

char **pkg_find(char *pkg_name) {
   char **paths = NULL;
   int  n = 0;
   char cwd[PATH_MAX];
   char **repos = REPOS;

   while (*repos) {
       if (chdir(*repos) != 0) {
           printf("error: Repository not accessible\n");       
           exit(1);
       }

       if (chdir(pkg_name) == 0) {
           paths = realloc(paths, sizeof(char*) * ++n);

           if (paths == NULL) {
               printf("Failed to allocate memory\n");
               exit(1);
           }

           paths[n - 1] =  strdup(getcwd(cwd, sizeof(cwd)));
       }

       ++repos;
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
