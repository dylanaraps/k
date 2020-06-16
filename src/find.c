#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
#include "pkg.h"

void pkg_find(package **head) {
   char **repos = REPOS;
   char *pwd;
   char cwd[PATH_MAX];

   head[0]->path_len = 0;
   head[0]->path = (char **) malloc(sizeof(char*) * 1);

   while (*repos) {
       if (chdir(*repos) != 0) {
           printf("error: Repository not accessible\n");       
           exit(1);
       }

       if (chdir(head[0]->name) == 0) {
           pwd = getcwd(cwd, sizeof(cwd));
           head[0]->path[head[0]->path_len] = malloc(sizeof(char) * ((strlen(pwd) + 1)));

           if (head[0]->path[head[0]->path_len] == NULL) {
               printf("Failed to allocate memory\n");
               exit(1);
           }

           strcpy(head[0]->path[++head[0]->path_len - 1], pwd);
       }

       ++repos;
   }

   chdir(PWD);
   head[0]->path[head[0]->path_len] = 0;

   if (head[0]->path_len == 0) {
       printf("error: %s not in any repository\n", head[0]->name);
       exit(1);
   }
}
