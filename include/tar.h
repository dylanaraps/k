/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_TAR_H
#define KISS_TAR_H

/**
 * Use libarchive for tar archives.
 */
#ifdef TAR_USE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>

/**
 * Fallback to executing tar utility if libarchive not available.
 */
#else


#endif

enum compression_type {
    TAR_NONE,
    TAR_BZ2,
    TAR_GZ,
    TAR_LZ,
    TAR_LZMA,
    TAR_XZ,
    TAR_ZSTD,
};

/**
 * Create a tar archive from contents of directory d, save to file f.
 * compression is one of the above enum values.
 */
int tar_create(const char *d, const char *f, int compression);

/**
 * Extract a tar archive to the current directory.
 */
int tar_extract(const char *f);

#endif
