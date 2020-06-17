#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>

#include <archive.h>
#include <archive_entry.h>

#include "pkg.h"
#include "log.h"
#include "tar.h"
#include "util.h"
#include "extract.h"


void pkg_extract(package *pkg) {
    char *src;
    int i;

    xchdir(MAK_DIR);
    mkdir(pkg->name, 0777);
    xchdir(pkg->name);

    if (pkg->src_len == 0)
        log_error("Sources file does not exist");

    for (i = 0; i < pkg->src_len; i++) {
        if (!pkg->source.src[i])
            log_error("Sources file does not exist");

        if (pkg->source.dest[i][0] != 0) {
            mkdir(pkg->source.dest[i], 0777);
            xchdir(pkg->source.dest[i]);
        }

        // TODO: match more than singular suffix.
        src = strrchr(pkg->source.src[i], '.');

        if (strcmp(src, ".tar")  == 0 ||
            strcmp(src, ".gz")   == 0 ||
            strcmp(src, ".tgz")  == 0 ||
            strcmp(src, ".xz")   == 0 ||
            strcmp(src, ".bz2")  == 0 ||
            strcmp(src, ".zst")  == 0 ||
            strcmp(src, ".lzma") == 0 ||
            strcmp(src, ".txz")  == 0 ||
            strcmp(src, ".lz")   == 0) {

            log_info("Extracting %s", pkg->source.src[i]);

            extract(pkg->source.src[i], 1, 
                ARCHIVE_EXTRACT_PERM  | 
                ARCHIVE_MATCH_MTIME   | 
                ARCHIVE_MATCH_CTIME   |
                ARCHIVE_EXTRACT_SECURE_NODOTDOT |
                ARCHIVE_EXTRACT_TIME
            );

        } else if (access(pkg->source.src[i], F_OK) != -1) {
            log_info("Copying    %s", pkg->source.src[i]);
            copy_file(pkg->source.src[i], basename(pkg->source.src[i]));

        } else {
            log_error("Source not found %s", pkg->source.src[i]);
        }

        xchdir(MAK_DIR);
        xchdir(pkg->name);
    }
}
