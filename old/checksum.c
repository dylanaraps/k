#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>

#include "log.h"
#include "util.h"
#include "vec.h"
#include "pkg.h"
#include "checksum.h"
#include "sha256.h"

void checksum_to_file(package *pkg) {
    FILE *file;

    file = fopenat(pkg->path, "checksums", O_RDWR | O_CREAT, "w");

    if (!file) {
        die("[%s] Failed to open checksums file", pkg->name);
    }

    for (size_t i = 0; i < vec_size(pkg->sum); i++) {
        fprintf(file, "%s\n", pkg->sum[i]);
    }

    fclose(file);
}

void pkg_checksums(package *pkg) {
    unsigned char buf[BUFSIZ];
    unsigned char shasum[32];
    char base[PATH_MAX];
    char *shastr;
    sha256_ctx ctx;
    FILE *file;
    int err;

    for (size_t i = 0; i < vec_size(pkg->src); i++) {
        file = fopen(pkg->src[i], "rb");

        if (!file) {
            die("[%s] Failed to read source (%s)",
                pkg->name, pkg->src[i]);
        }

        xstrlcpy(base, basename(pkg->src[i]), PATH_MAX);

        sha256_init(&ctx);

        while ((err = fread(buf, 1, sizeof(buf), file)) > 0) {
            sha256_update(&ctx, buf, err);
        }

        sha256_final(shasum, &ctx);

        /* 67 == 64 (shasum) + 2 ('  ') + 1 ('\0') */
        shastr = xmalloc(67 + strlen(base));

        for (int j = 0; j < 32; j++) {
            xsnprintf(&shastr[j * 2], 64, "%02x", shasum[j]);
        }
        xsnprintf(&shastr[64], strlen(base) + 3, "  %s", base);
        vec_push_back(pkg->sum, shastr);

        msg("%s", pkg->sum[i]);
        fclose(file);
    }
}

void pkg_verify(package *pkg) {
    FILE *file;
    char *line = 0;
    size_t i = 0;
    size_t src_l = vec_size(pkg->src);

    msg("[%s] Verifying checksums", pkg->name);
    pkg_checksums(pkg);

    file = fopenat(pkg->path, "checksums", O_RDONLY, "r");

    while ((getline(&line, &(size_t){0}, file) != -1)) {
        line[strcspn(line, "\n")] = 0;

        if (i > src_l || strcmp(pkg->sum[i], line) != 0) {
            die("[%s] Checksums mismatch", pkg->name);
        }

        i++;
    }

    fclose(file);
    free(line);

    msg("[%s] Verified checksums", pkg->name);
}
