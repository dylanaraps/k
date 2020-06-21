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
        die("Sources file does not exist");

    for (i = 0; i < pkg->src_len; i++) {
        if (!pkg->src[i])
            die("Sources file does not exist");

        if (pkg->dest[i][0] != 0) {
            mkdir(pkg->dest[i], 0777);
            xchdir(pkg->dest[i]);
        }

        src_len = strlen(pkg->src[i]);

        if (ends_with(pkg->src[i], ".tar",      src_len, 4) == 0 ||
            ends_with(pkg->src[i], ".tgz",      src_len, 4) == 0 ||
            ends_with(pkg->src[i], ".zip",      src_len, 4) == 0 ||
            ends_with(pkg->src[i], ".txz",      src_len, 4) == 0 ||
            ends_with(pkg->src[i], ".tar.gz",   src_len, 7) == 0 ||
            ends_with(pkg->src[i], ".tar.lz",   src_len, 7) == 0 ||
            ends_with(pkg->src[i], ".tar.xz",   src_len, 7) == 0 ||
            ends_with(pkg->src[i], ".tar.bz2",  src_len, 8) == 0 ||
            ends_with(pkg->src[i], ".tar.zst",  src_len, 8) == 0 ||
            ends_with(pkg->src[i], ".tar.lzma", src_len, 9) == 0) {

            msg("[%s] Extracting %s", pkg->name, pkg->src[i]);

            extract(pkg->src[i], 1,
                ARCHIVE_EXTRACT_PERM |
                ARCHIVE_MATCH_MTIME |
                ARCHIVE_MATCH_CTIME |
                ARCHIVE_EXTRACT_SECURE_NODOTDOT |
                ARCHIVE_EXTRACT_TIME
            );

        } else if (access(pkg->src[i], F_OK) != -1) {
            msg("[%s] Copying %s", pkg->name, pkg->src[i]);
            copy_file(pkg->src[i], basename(pkg->src[i]));

        } else {
            die("[%s] Source not found %s", pkg->name, pkg->src[i]);
        }

        xchdir(MAK_DIR);
        xchdir(pkg->name);
    }
}
