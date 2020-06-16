#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include "sha.h"
#include "checksum.h"
#include "pkg.h"

char **pkg_checksums(package pkg) {
   char **repos = pkg.path; 
   FILE *file;
   char *lbuf = 0;
   char **res = NULL;
   char *source;
   char *source_file;
   FILE *src;
   unsigned char buf[1000];
   unsigned char shasum[32];
   sha256_ctx ctx;
   int i;
   int sbuf_size;

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

       sha256_init(&ctx);
       while ((i = fread(buf, 1, sizeof(buf), src)) > 0) {
           sha256_update(&ctx, buf, i );
       }
       sha256_final(&ctx, shasum);

       sbuf_size = 67 * sizeof(int) + strlen(source_file);
       char sbuf[sbuf_size];

       sprintf(sbuf, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
%02x%02x%02x%02x%02x%02x%02x%02xx%02x%02x%02x  %s", sbuf_size, 
           shasum[0],  shasum[1],  shasum[2],  shasum[3],
           shasum[4],  shasum[5],  shasum[6],  shasum[7],
           shasum[8],  shasum[9],  shasum[10], shasum[11],
           shasum[12], shasum[13], shasum[14], shasum[15],
           shasum[16], shasum[17], shasum[18], shasum[19],
           shasum[20], shasum[21], shasum[22], shasum[23],
           shasum[24], shasum[25], shasum[26], shasum[27],
           shasum[28], shasum[29], shasum[30], shasum[31],
           source_file
       );

       fprintf(stderr, "%s\n", sbuf);

       fclose(src);
       chdir(SRC_DIR);
   }

   free(lbuf);
   fclose(file);
   return res;
}
