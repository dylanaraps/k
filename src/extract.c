#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>

#include <archive.h>
#include <archive_entry.h>

#include "log.h"
#include "util.h"
#include "file.h"
#include "cache.h"
#include "pkg.h"
#include "extract.h"

static void tar_copy(struct archive *ar, struct archive *aw) {
    const void *buf;
    size_t len;
    int64_t off;
    int err;

    for (;;) {
        err = archive_read_data_block(ar, &buf, &len, &off);

        if (err == ARCHIVE_EOF) {
            return;
        }

        if (err != ARCHIVE_OK) {
            die("Failed to extract archive");
        }

        err = archive_write_data_block(aw, buf, len, off);

        if (err != ARCHIVE_OK) {
            die("Failed to extract archive");
        }
    }
}

static void tar_extract(const char *file) {
    struct archive *arc;
    struct archive *ext;
    struct archive_entry *ent;
    const char *path;
    int err;

    if (!file) {
        die("Invalid input to tar extraction");
    }

    arc = archive_read_new();
    ext = archive_write_disk_new();

    archive_write_disk_set_options(ext,
        ARCHIVE_EXTRACT_PERM |
        ARCHIVE_MATCH_MTIME |
        ARCHIVE_MATCH_CTIME |
        ARCHIVE_EXTRACT_SECURE_NODOTDOT |
        ARCHIVE_EXTRACT_TIME
    );

    archive_read_support_format_tar(arc);
    archive_read_support_filter_all(arc);

    err = archive_read_open_filename(arc, file, 10240);

    if (err == ARCHIVE_FATAL) {
        die("Failed to read tar archive");
    }

    for (;;) {
        err = archive_read_next_header(arc, &ent);

        if (err == ARCHIVE_EOF) {
            break;
        }

        if (err != ARCHIVE_OK) {
            die("Failed to read tar archive");
        }

        path = archive_entry_pathname(ent);

        /* strip first path component */
        if (path) {
            path = strchr(path, '/');

            if (path && path[0] == '/') {
                ++path;
            }

            if (path) {
                archive_entry_set_pathname(ent, path);
            }
        }

        err = archive_write_header(ext, ent);

        /* dying here if ERR causes all extractions to fail... */
        if (err == ARCHIVE_OK) {
            tar_copy(arc, ext);
        }

        err = archive_write_finish_entry(ext);

        if (err != ARCHIVE_OK) {
            die("Failed to read tar archive");
        }
    }

    archive_read_close(arc);
    archive_read_free(arc);
    archive_write_close(ext);
    archive_write_free(ext);
}

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
            tar_extract(pkg->src[i]);

        } else if (access(pkg->src[i], F_OK) != -1) {
            msg("[%s] Copying %s", pkg->name, pkg->src[i]);
            cp_file(pkg->src[i], basename(pkg->src[i]));

        } else {
            die("[%s] Source not found %s", pkg->name, pkg->src[i]);
        }

        if (chdir(pkg->mak_dir) != 0) {
            die("[%s] Build directory not accessible", pkg->name);
        }
    }
}
