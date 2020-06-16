#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pkg.h"
#include "extract.h"

void pkg_extract(package *pkg) {
    char *src;
    int i;

    chdir(TAR_DIR);

    if (pkg->src_len == 0) {
        printf("error: no sources\n");
        exit(1);
    }

    for (i = 0; i < pkg->src_len; i++) {
        src = strrchr(pkg->source.src[i], '.');

        if (!src) {
            printf("error: Source doesn't exist\n");
            exit(1);

        } else if (strcmp(src, ".tar")      == 0 ||
                   strcmp(src, ".gz")   == 0 ||
                   strcmp(src, ".xz")   == 0 ||
                   strcmp(src, ".bz2")  == 0 ||
                   strcmp(src, ".zst")  == 0 ||
                   strcmp(src, ".lzma") == 0 ||
                   strcmp(src, ".txz")  == 0 ||
                   strcmp(src, ".lz")   == 0) {
            
        }
    }
}

