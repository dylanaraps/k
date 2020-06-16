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

void pkg_checksums(package *pkg) {
   FILE *src;
   unsigned char buf[1000];
   unsigned char shasum[32];
   char *base;
   sha256_ctx ctx;
   int i;
   int j;

   pkg->sums = (char **) malloc(sizeof(char *) * pkg->src_len + 1);

   for (i = 0; i < pkg->src_len; i++) {
       src  = fopen(pkg->source.src[i], "rb");
       base = basename(pkg->source.src[i]);

       if (!src) {
           printf("error: Failed to generate checksums\n");
           exit(1);
       }

       pkg->sums[i] = malloc(67 * sizeof(char) + strlen(base));

       if (!pkg->sums[i]) {
           printf("error: Failed to allocate memory\n");
           exit(1);
       }

       sha256_init(&ctx);
       while ((j = fread(buf, 1, sizeof(buf), src)) > 0) {
           sha256_update(&ctx, buf, j);
       }
       sha256_final(&ctx, shasum);

       sprintf(pkg->sums[i], "%02x%02x%02x%02x%02x%02x\
%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x  %s",  
           shasum[0],  shasum[1],  shasum[2],  shasum[3],
           shasum[4],  shasum[5],  shasum[6],  shasum[7],
           shasum[8],  shasum[9],  shasum[10], shasum[11],
           shasum[12], shasum[13], shasum[14], shasum[15],
           shasum[16], shasum[17], shasum[18], shasum[19],
           shasum[20], shasum[21], shasum[22], shasum[23],
           shasum[24], shasum[25], shasum[26], shasum[27],
           shasum[28], shasum[29], shasum[30], shasum[31],
           base
       );

       printf("%s\n", pkg->sums[i]);
       fclose(src);
   }

   pkg->sums[i + 1] = 0;
}
