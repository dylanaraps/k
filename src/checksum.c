#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>

#include "log.h"
#include "util.h"
#include "pkg.h"
#include "checksum.h"
#include "sha256.h"

void checksum_to_file(package *pkg) {
    FILE *file;
    int i;

    file = fopenat(pkg->path, "checksums", O_RDWR | O_CREAT, "w");

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

        xstrlcpy(base, basename(pkg->src[pkg->sum_l]), PATH_MAX);

        sha256_init(&ctx);

        while ((err = fread(buf, 1, sizeof(buf), file)) > 0) {
            sha256_update(&ctx, buf, err);
        }

        sha256_final(shasum, &ctx);

        /* 67 == 64 (shasum) + 2 ('  ') + 1 ('\0') */
        pkg->sum[pkg->sum_l] = xmalloc(67 + strlen(base));

        for (int i = 0; i < 32; i++) {
            xsnprintf(&pkg->sum[pkg->sum_l][i * 2], 64, "%02x", shasum[i]);
        }
        xsnprintf(&pkg->sum[pkg->sum_l][64], strlen(base) + 3, "  %s", base);

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

    file = fopenat(pkg->path, "checksums", O_RDONLY, "r");

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
