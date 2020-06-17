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
#include "tar.h"
#include "extract.h"


void pkg_extract(package *pkg) {
    char *src;
    char *dest;
    int i;

    if (chdir(MAK_DIR) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }

    mkdir(pkg->name, 0777);

    if (chdir(pkg->name) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }

    if (pkg->src_len == 0) {
        printf("error: no sources\n");
        exit(1);
    }

    for (i = 0; i < pkg->src_len; i++) {
        if (!pkg->source.src[i]) {
            printf("error: Source doesn't exist\n");
            exit(1);
        }

        if (pkg->source.dest[i][0] != 0) {
            mkdir(pkg->source.dest[i], 0777);

            if (chdir(pkg->source.dest[i]) != 0) {
                printf("error: Dest not accessible\n");
                exit(1);
            }
        }

        src = strrchr(pkg->source.src[i], '.');

        if (strcmp(src, ".tar")  == 0 ||
            strcmp(src, ".gz")   == 0 ||
            strcmp(src, ".xz")   == 0 ||
            strcmp(src, ".bz2")  == 0 ||
            strcmp(src, ".zst")  == 0 ||
            strcmp(src, ".lzma") == 0 ||
            strcmp(src, ".txz")  == 0 ||
            strcmp(src, ".lz")   == 0) {
            printf("Extracting %s...\n", pkg->source.src[i]);
            extract(pkg->source.src[i], 1, ARCHIVE_EXTRACT_PERM);

        } else if (access(pkg->source.src[i], F_OK) != -1) {
            int in_fd;
            int out_fd;
            int err;
            unsigned char buffer[4096];
            char *dest = basename(pkg->source.src[i]);

            printf("Copying %s\n", pkg->source.src[i]);
            in_fd = open(pkg->source.src[i], O_RDONLY);

            if (in_fd == -1) {
                printf("error: File not accessible %s\n", pkg->source.src[i]);
                exit(1);
            }

            out_fd = open(dest, O_CREAT | O_WRONLY, 0666);

            if (out_fd == -1) {
                printf("error: Cannot copy file %s\n", pkg->source.src[i]);
                exit(1);
            }

            while (1) {
                err = read(in_fd, buffer, 4096); 

                if (err == -1) {
                    printf("error: File not accessible\n");
                    exit(1);

                } else if (err == 0) {
                    break;
                }

                err = write(out_fd, buffer, err);

                if (err == -1) {
                    printf("error: Cannot copy file\n");
                    exit(1);
                }
            }

            close(in_fd);
            close(out_fd);

        } else {
            printf("error: Source not found %s\n", pkg->source.src[i]);
            exit(1);
        }

        chdir(MAK_DIR);
        chdir(pkg->name);
    }
}
