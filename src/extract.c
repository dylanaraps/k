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
#include "extract.h"


void pkg_extract(package *pkg) {
    int in_fd;
    int out_fd;
    int err;
    unsigned char buffer[4096];
    char *src;
    char *dest;
    int i;

    if (chdir(MAK_DIR) != 0) {
        log_error("Cache directory not accessible");
    }

    mkdir(pkg->name, 0777);

    if (chdir(pkg->name) != 0) {
        log_error("Cache directory not accessible");
    }

    if (pkg->src_len == 0) {
        log_error("Sources file does not exist");
    }

    for (i = 0; i < pkg->src_len; i++) {
        if (!pkg->source.src[i]) {
            log_error("Sources file does not exist");
        }

        if (pkg->source.dest[i][0] != 0) {
            mkdir(pkg->source.dest[i], 0777);

            if (chdir(pkg->source.dest[i]) != 0) {
                log_error("Dest not accessible (%s)", pkg->source.dest[i]);
            }
        }

        src = strrchr(pkg->source.src[i], '.');

        if (strcmp(src, ".tar")  == 0 ||
            strcmp(src, ".gz")   == 0 ||
            strcmp(src, ".tgz")  == 0 ||
            strcmp(src, ".xz")   == 0 ||
            strcmp(src, ".bz2")  == 0 ||
            strcmp(src, ".zst")  == 0 ||
            strcmp(src, ".lzma") == 0 ||
            strcmp(src, ".txz")  == 0 ||
            strcmp(src, ".lz")   == 0) {

            log_info("Extracting %s", pkg->source.src[i]);
            extract(pkg->source.src[i], 1, ARCHIVE_EXTRACT_PERM | 
                                           ARCHIVE_MATCH_MTIME | 
                                           ARCHIVE_MATCH_CTIME);

        } else if (access(pkg->source.src[i], F_OK) != -1) {
            dest = basename(pkg->source.src[i]);

            log_info("Copying %s", pkg->source.src[i]);
            in_fd = open(pkg->source.src[i], O_RDONLY);

            if (in_fd == -1) {
                log_error("File not accessible %s\n", pkg->source.src[i]);
            }

            out_fd = open(dest, O_CREAT | O_WRONLY, 0666);

            if (out_fd == -1) {
                log_error("Cannot copy file %s\n", pkg->source.src[i]);
            }

            while (1) {
                err = read(in_fd, buffer, 4096); 

                if (err == -1) {
                    log_error("File not accessible %s\n", pkg->source.src[i]);

                } else if (err == 0) {
                    break;
                }

                err = write(out_fd, buffer, err);

                if (err == -1) {
                    log_error("Cannot copy file %s\n", pkg->source.src[i]);
                }
            }

            close(in_fd);
            close(out_fd);

        } else {
            log_fatal("Source not found %s", pkg->source.src[i]);
        }

        chdir(MAK_DIR);
        chdir(pkg->name);
    }
}
