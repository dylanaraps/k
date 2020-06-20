#include "log.h"
#include "util.h"
#include "pkg.h"
#include "checksum.h"

void pkg_checksums(package *pkg) {
    FILE *file;
    int i;

    pkg->sum = xmalloc((pkg->src_l + 1) * sizeof(char *));

    for (i = 0; i < pkg->src_l; i++) {
        file = fopen(pkg->src[i], "rb");

        if (!file) {
            die("[%s] Failed to read source (%s)", pkg->name, pkg->src[i]);
        }

        fclose(file);
    }
}
