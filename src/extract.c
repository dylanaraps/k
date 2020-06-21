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
    int i;

    if (chdir(pkg->mak_dir) != 0) {
        die("[%s] Build directory not accessible", pkg->name);
    }

    if (pkg->src_l == 0) {
        die("[%s] Empty sources file", pkg->name);
    }

    for (i = 0; i < pkg->src_l; i++) {
        if (!pkg->src[i]) {
            die("[%s] Invalid sources file", pkg->name);
        }

        if (pkg->des[i][0]) {
            mkdir_p(pkg->des[i]);

            if (chdir(pkg->des[i]) != 0) {
                die("[%s] Source destination not accessible", pkg->name);
            }
        }

        if (strsuf(pkg->src[i], ".tar",      4) == 0 ||
            strsuf(pkg->src[i], ".tgz",      4) == 0 ||
            strsuf(pkg->src[i], ".zip",      4) == 0 ||
            strsuf(pkg->src[i], ".txz",      4) == 0 ||
            strsuf(pkg->src[i], ".tar.gz",   7) == 0 ||
            strsuf(pkg->src[i], ".tar.lz",   7) == 0 ||
            strsuf(pkg->src[i], ".tar.xz",   7) == 0 ||
            strsuf(pkg->src[i], ".tar.bz2",  8) == 0 ||
            strsuf(pkg->src[i], ".tar.zst",  8) == 0 ||
            strsuf(pkg->src[i], ".tar.lzma", 9) == 0) {

            msg("[%s] Extracting %s", pkg->name, pkg->src[i]);

        } else if (access(pkg->src[i], F_OK) != -1) {
            msg("[%s] Copying %s", pkg->name, pkg->src[i]);

        } else {
            die("[%s] Source not found %s", pkg->name, pkg->src[i]);
        }

        if (chdir(pkg->mak_dir) != 0) {
            die("[%s] Build directory not accessible", pkg->name);
        }
    }
}
