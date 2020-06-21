#define _POSIX_C_SOURCE 200809L
#include <unistd.h> /* chdir */
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strlen */

#include "log.h"
#include "util.h"
#include "cache.h"
#include "pkg.h"
#include "extract.h"

void pkg_extract(package *pkg) {
    size_t src_len;
    int i;

    if (chdir(pkg->mak_dir) != 0) {
        die("Build directory not accessible");
    }

    if (pkg->src_l == 0) {
        die("Empty sources file");
    }

    for (i = 0; i < pkg->src_l; i++) {
        if (!pkg->src[i]) {
            die("Invalid sources file");
        }

        if (pkg->des[i][0]) {
            mkdir_p(pkg->des[i]);

            if (chdir(pkg->des[i]) != 0) {
                die("Source destination not accessible");
            }
        }

        src_len = strlen(pkg->src[i]) + 1;

        if (strsuf(pkg->src[i], ".tar",      src_len, 4) == 0 ||
            strsuf(pkg->src[i], ".tgz",      src_len, 4) == 0 ||
            strsuf(pkg->src[i], ".zip",      src_len, 4) == 0 ||
            strsuf(pkg->src[i], ".txz",      src_len, 4) == 0 ||
            strsuf(pkg->src[i], ".tar.gz",   src_len, 7) == 0 ||
            strsuf(pkg->src[i], ".tar.lz",   src_len, 7) == 0 ||
            strsuf(pkg->src[i], ".tar.xz",   src_len, 7) == 0 ||
            strsuf(pkg->src[i], ".tar.bz2",  src_len, 8) == 0 ||
            strsuf(pkg->src[i], ".tar.zst",  src_len, 8) == 0 ||
            strsuf(pkg->src[i], ".tar.lzma", src_len, 9) == 0) {

            msg("[%s] Extracting %s", pkg->name, pkg->src[i]);
        }
    }
}
