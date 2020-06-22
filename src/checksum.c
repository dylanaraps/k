#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* getline */
#include <libgen.h> /* basename */
#include <string.h> /* strlen */
#include <unistd.h> /* chdir */

#include "log.h"
#include "util.h"
#include "strl.h"
#include "pkg.h"
#include "checksum.h"
#include "sha256.h"

void checksum_to_file(package *pkg) {
    FILE *file;
    int i;

    file = fopenat(pkg->path, "checksums", "w");

    if (!file) {
        die("[%s] Failed to open checksums file", pkg->name);
    }

    for (i = 0; i < pkg->src_l; i++) {
        fprintf(file, "%s\n", pkg->sum[i]);
    }

    fclose(file);
}

void pkg_checksums(package *pkg) {
    unsigned char buf[BUFSIZ];
    unsigned char shasum[32];
    char base[PATH_MAX];
    sha256_ctx ctx;
    FILE *file;
    int err;

    pkg->sum = xmalloc((pkg->src_l + 1) * sizeof(char *));

    for (pkg->sum_l = 0; pkg->sum_l < pkg->src_l; pkg->sum_l++) {
        file = fopen(pkg->src[pkg->sum_l], "rb");

        if (!file) {
            die("[%s] Failed to read source (%s)",
                pkg->name, pkg->src[pkg->sum_l]);
        }

        err = strlcpy(base, basename(pkg->src[pkg->sum_l]), PATH_MAX);

        if (err >= PATH_MAX) {
            die("strlcpy failed");
        }

        sha256_init(&ctx);

        while ((err = fread(buf, 1, sizeof(buf), file)) > 0) {
            sha256_update(&ctx, buf, err);
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
}

void pkg_verify(package *pkg) {
    FILE *file;
    char *line = 0;
    int i = 0;

    msg("[%s] Verifying checksums", pkg->name);
    pkg_checksums(pkg);

    if (pkg->src_l == 0) {
        die("[%s] Sources file does not exist", pkg->name);
    }

    file = fopenat(pkg->path, "checksums", "r");

    while ((getline(&line, &(size_t){0}, file) != -1)) {
        line[strcspn(line, "\n")] = 0;

        if (i > pkg->src_l || strcmp(pkg->sum[i], line) != 0) {
            die("[%s] Checksums mismatch", pkg->name);
        }

        i++;
    }

    fclose(file);
    free(line);

    msg("[%s] Verified checksums", pkg->name);
}
