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
    int i;
    int src_len;

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

        src_len = strlen(pkg->source.src[i]);

        if (ends_with(pkg->source.src[i], ".tar",      src_len, 4) == 0 ||
            ends_with(pkg->source.src[i], ".tgz",      src_len, 4) == 0 ||
            ends_with(pkg->source.src[i], ".zip",      src_len, 4) == 0 ||
            ends_with(pkg->source.src[i], ".txz",      src_len, 4) == 0 ||
            ends_with(pkg->source.src[i], ".tar.gz",   src_len, 7) == 0 ||
            ends_with(pkg->source.src[i], ".tar.lz",   src_len, 7) == 0 || 
            ends_with(pkg->source.src[i], ".tar.xz",   src_len, 7) == 0 ||
            ends_with(pkg->source.src[i], ".tar.bz2",  src_len, 8) == 0 ||
            ends_with(pkg->source.src[i], ".tar.zst",  src_len, 8) == 0 ||
            ends_with(pkg->source.src[i], ".tar.lzma", src_len, 9) == 0) {

            log_info("Extracting %s", pkg->source.src[i]);

            extract(pkg->source.src[i], 1, 
                ARCHIVE_EXTRACT_PERM | 
                ARCHIVE_MATCH_MTIME | 
                ARCHIVE_MATCH_CTIME |
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
