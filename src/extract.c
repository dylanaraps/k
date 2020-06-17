#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <archive.h>
#include <archive_entry.h>

#include "pkg.h"
#include "tar.h"
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
        if (!pkg->source.src[i]) {
            printf("error: Source doesn't exist\n");
            exit(1);
        }

        src = strrchr(pkg->source.src[i], '.');

        if (strcmp(src, ".tar")      == 0 ||
                   strcmp(src, ".gz")   == 0 ||
                   strcmp(src, ".xz")   == 0 ||
                   strcmp(src, ".bz2")  == 0 ||
                   strcmp(src, ".zst")  == 0 ||
                   strcmp(src, ".lzma") == 0 ||
                   strcmp(src, ".txz")  == 0 ||
                   strcmp(src, ".lz")   == 0) {
            printf("Extracting %s...\n", pkg->source.src[i]);
            extract(pkg->source.src[i], 1, ARCHIVE_EXTRACT_PERM);
        }
    }
}
