/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_TAR_H
#define KISS_TAR_H

/**
 * Use libarchive for tar archives.
 */
#ifdef USE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>

/**
 * Fallback to executing tar utility if libarchive not available.
 */
#else


#endif

/**
 * Extract a tar archive to the current directory.
 */
int tar_extract(const char *f, int flags);

#endif
