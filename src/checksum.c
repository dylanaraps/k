#include <libgen.h> /* basename */
#include <string.h> /* strlen */
#include <unistd.h> /* chdir */

#include "log.h"
#include "util.h"
#include "strl.h"
#include "pkg.h"
#include "checksum.h"
#include "sha256.h"

static void checksum_to_file(package *pkg) {
    FILE *file;
    int i;

    if (chdir(pkg->path[0]) != 0) {
        die("Repository files not accessible");
    }

    file = fopen("checksums", "w");

    if (!file) {
        die("Failed to open checksums file");
    }

    for (i = 0; i < pkg->src_l; i++) {
        fprintf(file, "%s\n", pkg->sum[i]);
    }

    fclose(file);
}

void pkg_checksums(package *pkg) {
    unsigned char buf[1000];
    unsigned char shasum[32];
    char base[PATH_MAX];
    sha256_ctx ctx;
    FILE *file;
    int i;

    pkg->sum = xmalloc((pkg->src_l + 1) * sizeof(char *));

    for (pkg->sum_l = 0; pkg->sum_l < pkg->src_l; pkg->sum_l++) {
        file = fopen(pkg->src[pkg->sum_l], "rb");
        strlcpy(base, basename(pkg->src[pkg->sum_l]), PATH_MAX);

        if (!file) {
            die("[%s] Failed to read source (%s)",
                pkg->name, pkg->src[pkg->sum_l]);
        }

        sha256_init(&ctx);

        while ((i = fread(buf, 1, sizeof(buf), file)) > 0) {
            sha256_update(&ctx, buf, i);
        }

        sha256_final(shasum, &ctx);

        /* 67 == 64 (shasum) + 2 ('  ') + 1 ('\0') */
        pkg->sum[pkg->sum_l] = xmalloc(67 + strlen(base));

        snprintf(pkg->sum[pkg->sum_l], 67 + strlen(base), "%02x%02x%02x%02x%02\
x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
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

        msg("%s", pkg->sum[pkg->sum_l]);
        fclose(file);
    }

    checksum_to_file(pkg);
}
