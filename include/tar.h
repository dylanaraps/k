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

int tar_extract(const char *f, int flags);

#endif
