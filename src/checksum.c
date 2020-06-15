#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include "sha.h"
#include "pkg.h"

void pkg_checksums(package pkg) {
   char **repos = pkg_find(pkg.name); 
   FILE *file;
   char *lbuf = 0;
   char *source;
   char *source_file;
   FILE *src;
   unsigned char buf[1000];
   unsigned char shasum[32];
   sha256_context ctx;
   int i;
   int j;

   chdir(*repos);
   file = fopen("sources", "r");
   
   if (chdir(SRC_DIR) != 0) {
       printf("error: Sources directory not accessible\n"); 
       exit(1);
   }

   if (!file) {
       printf("error: Sources file invalid\n");
       exit(1);
   }

   while ((getline(&lbuf, &(size_t){0}, file)) > 0) {
       // Skip comments and blank lines.
       if ((lbuf)[0] == '#' || (lbuf)[0] == '\n') {
           continue;
       }

       source      = strtok(lbuf, " 	\n");
       source_file = basename(source);

       if (chdir(pkg.name) != 0) {
           printf("error: Sources directory not accessible\n");
           exit(1);
       }

       if (access(source_file, F_OK) == -1) {
           chdir(*repos);
           chdir(dirname(source));

           if (access(source_file, F_OK) == -1) {
               printf("error: Source not accessible %s\n", source_file);
               exit(1);
           }
       }

       src = fopen(source_file, "rb");

       if (!src) {
           printf("error: Failed to generate checksums\n");
           exit(1);
       }

       sha256_starts(&ctx);

       while ((i = fread(buf, 1, sizeof(buf), src)) > 0) {
           sha256_update(&ctx, buf, i );
       }

       sha256_finish(&ctx, shasum);

       for(j = 0; j < 32; j++) {
           printf("%02x", shasum[j] );
       }
       printf("  %s\n", source_file);

       chdir(SRC_DIR);
   }

   fclose(file);
}
