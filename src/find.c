#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "pkg.h"

char **pkg_find(char *pkg_name) {
   char **repos = REPOS;
   char **paths;
   char *pwd;
   char cwd[PATH_MAX];
   int n = 0;

   paths = (char **) malloc(sizeof(char*) * 1);

   if (*paths == NULL) {
       printf("Failed to allocate memory\n");
       exit(1);
   }

   while (*repos) {
       if (chdir(*repos) != 0) {
           printf("error: Repository not accessible\n");       
           exit(1);
       }

       if (chdir(pkg_name) == 0) {
           pwd = getcwd(cwd, sizeof(cwd));
           paths[n] = malloc(sizeof(char) * ((strlen(pwd) + 1)));

           if (paths[n] == NULL) {
               printf("Failed to allocate memory\n");
               exit(1);
           }

           strcpy(paths[++n - 1], pwd);
       }

       ++repos;
   }

   chdir(PWD);
   paths[n] = 0;

   if (*paths) {
       return paths;

   } else {
       printf("error: %s not in any repository\n", pkg_name);
       exit(1);
   }
}
