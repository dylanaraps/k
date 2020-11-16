#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "arr.h"
#include "buf.h"
#include "error.h"
#include "util.h"
#include "tar.h"

/**
 * Use libarchive for tar archives.
 */
#ifdef USE_LIBARCHIVE

static const int archive_opts = \
    // Refuse to extract an absolute path.
    ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS |

    // Refuse to extract path containing ..
    ARCHIVE_EXTRACT_SECURE_NODOTDOT |

    // Refuse to extract any object whose final location would be altered
    // by a symlink on disk
    ARCHIVE_EXTRACT_SECURE_SYMLINKS
;

static int tar_write_data(struct archive *r, struct archive *w) {
    const void *buf = 0;
    int64_t off = 0;
    size_t l = 0;

    for (int ret = 0; ;) {
        if ((ret = archive_read_data_block(r, &buf, &l, &off)) == ARCHIVE_EOF) {
            break;
        }

        if (ret != ARCHIVE_OK) {
            err("failed to extract archive: %s", archive_error_string(r));
            return -1;
        }

        if ((ret = archive_write_data_block(w, buf, l, off)) != ARCHIVE_OK) {
            err("failed to extract archive: %s", archive_error_string(r));
            return -1;
        }
    }

    return 0;
}

static int tar_strip_component(struct archive_entry *e) {
    const char *path = archive_entry_pathname(e);

    if (!path) {
        err("failed to allocate memory");
        return -1;
    }

    if (strchr(path, '/')) {
        path = strchr(path, '/');
    }

    if (*path == '/') {
        path++;
    }

    archive_entry_set_pathname(e, path);
    return 0;
}

static int tar_enable_compression(struct archive *a, int compress) {
    switch (compress) {
#ifdef ARCHIVE_FILTER_BZIP2
        case TAR_BZ2:
            return archive_write_add_filter_bzip2(a);
#endif

#ifdef ARCHIVE_FILTER_GZIP
        case TAR_GZ:
            return archive_write_add_filter_gzip(a);
#endif

#ifdef ARCHIVE_FILTER_XZ
        case TAR_XZ:
            return archive_write_add_filter_xz(a);
#endif

#ifdef ARCHIVE_FILTER_LZIP
        case TAR_LZ:
            return archive_write_add_filter_lzip(a);
#endif

#ifdef ARCHIVE_FILTER_LZMA
        case TAR_LZMA:
            return archive_write_add_filter_lzma(a);
#endif

#ifdef ARCHIVE_FILTER_ZSTD
        case TAR_ZSTD:
            return archive_write_add_filter_zstd(a);
#endif

        case TAR_NONE:
            return ARCHIVE_OK;
    }

    err("no compression method found");
    err("was libarchive configured correctly?");
    return -1;
}

static int tar_write_file(struct archive *w, const char *f) {
    int ret = 0;
    struct archive *r = archive_read_disk_new();

    if (!r) {
        err("failed to allocate memory");
        return -1;
    }

    if ((ret = archive_read_disk_set_standard_lookup(r)) != ARCHIVE_OK) {
        err("failed to read file: %s", archive_error_string(r));
        goto r_error;
    }

    if ((ret = archive_read_disk_open(r, f)) != ARCHIVE_OK) {
        err("failed to open file: %s", archive_error_string(r));
        goto r_error;
    }

    for (struct archive_entry *e = 0; ;) {
        if (!(e = archive_entry_new())) {
            err("failed to allocate memory");
            ret = -1;
            goto r_error;
        }

        ret = archive_read_next_header2(r, e);

        if (ret == ARCHIVE_EOF) {
            ret = 0;
            goto f_error;
        }

        if (ret != ARCHIVE_OK) {
            err("failed to read header: %s", archive_error_string(r));
            goto f_error;
        }

        archive_read_disk_descend(r);

        if ((ret = archive_write_header(w, e)) != ARCHIVE_OK) {
            err("failed to write header: %s", archive_error_string(w));
            goto f_error;
        }

        int fd = open(archive_entry_sourcepath(e), O_RDONLY);

        if (fd == -1) {
            err_no("failed to open path");
            goto f_error;
        }

        char m[4096];
        for (ssize_t l; (l = read(fd, m, sizeof(m))) > 0;) {
            if ((ret = archive_write_data(w, m, l)) == -1) {
                err("failed to write data: %s", archive_error_string(w));
                goto d_error;
            }
        }

        close(fd);
        archive_entry_free(e);
        continue;

d_error:
        close(fd);

f_error:
        archive_entry_free(e);
        break;
    }

r_error:
    archive_read_close(r);
    archive_read_free(r);
    return ret;
}

int tar_create(const char *d, const char *f, int compression) {
    struct archive *w = archive_write_new();

    if (!w) {
        err("failed to allocate memory");
        return -1;
    }

    int ret = 0;

    if ((ret = tar_enable_compression(w, compression)) != ARCHIVE_OK) {
        err("failed to enable compression: %s", archive_error_string(w));
        goto w_error;
    }

    if ((ret = archive_write_set_format_ustar(w)) != ARCHIVE_OK) {
        err("failed to set format: %s", archive_error_string(w));
        goto w_error;
    }

    if ((ret = archive_write_open_filename(w, f)) != ARCHIVE_OK) {
        err("failed to create archive: %s", archive_error_string(w));
        goto w_error;
    }

    DIR *dir = opendir(d);

    if (!dir) {
        err_no("failed to open directory '%s'", d);
        ret = -1;
        goto w_error;
    }

    buf *mem = buf_alloc(0, 256);

    if (!mem) {
        err("failed to allocate memory");
        ret = -1;
        goto w_error;
    }

    if ((ret = buf_push_s(&mem, d)) < 0) {
        err_no("buf error");
        goto b_error;
    }

    buf_rstrip(&mem, '/');
    buf_push_c(&mem, '/');
    buf_push_c(&mem, 0);

    size_t len_pre = buf_len(mem);

    for (struct dirent *dp; (dp = readdir(dir)); ) {
        if (dp->d_name[0] == '.' && (!dp->d_name[1] ||
           (dp->d_name[1] == '.' && !dp->d_name[2]))) {
            continue;
        }

        if ((ret = buf_push_s(&mem, dp->d_name)) < 0) {
            err_no("buf erorr");
            break;
        }

        if ((ret = tar_write_file(w, mem)) != 0) {
            err("failed to read dirent: %s", archive_error_string(w));
            break;
        }

        buf_set_len(&mem, len_pre);
    }

b_error:
    buf_free(&mem);
    closedir(dir);

w_error:
    archive_write_close(w);
    archive_write_free(w);

    return ret;
}

int tar_extract(const char *f) {
    struct archive *r = archive_read_new();

    if (!r) {
        return -1;
    }

    int ret = 0;
    struct archive *w = archive_write_disk_new();

    if (!w) {
        ret = -1;
        goto r_error;
    }

    archive_write_disk_set_options(w, archive_opts);
    archive_read_support_format_all(r);
    archive_read_support_filter_all(r);

    if ((ret = archive_write_disk_set_standard_lookup(w)) != ARCHIVE_OK) {
        err("failed to extract archive: %s", archive_error_string(w));
        goto w_error;
    }

    if ((ret = archive_read_open_filename(r, f, 10240)) != ARCHIVE_OK) {
        err("failed to open archive: %s", archive_error_string(r));
        goto w_error;
    }

    for (struct archive_entry *e = 0; ;) {
        if ((ret = archive_read_next_header(r, &e)) == ARCHIVE_EOF) {
            ret = 0;
            break;
        }

        if (ret != ARCHIVE_OK) {
            err("failed to extract archive: %s", archive_error_string(r));
            break;
        }

        if ((ret = tar_strip_component(e)) < 0) {
            break;
        }

        archive_write_header(w, e);

        if ((ret = tar_write_data(r, w)) < 0) {
            break;
        }
    }

w_error:
    archive_write_close(w);
    archive_write_free(w);
r_error:
    archive_read_close(r);
    archive_read_free(r);

    return ret;
}

/**
 * Fallback to executing tar utility if libarchive not available.
 *
 * Requires a "smart" tar that is aware of compression and can automagically
 * deal with it (GNU tar, libarchive tar (BSDtar), busybox tar, ...).
 */
#else

int tar_create(const char *d, const char *f, int compression) {
    (void) compression;

    const char *cmd[] = {
        "tar", "acf", f, d, 0
    };

    return run_cmd((char **) cmd);
}

int tar_extract(const char *f) {
    const char *cmd[] = {
        "tar", "xf", f, "--strip-components", "1", 0
    };

    return run_cmd((char **) cmd);
}

#endif

