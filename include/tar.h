#ifndef KISS_TAR_H
#define KISS_TAR_H

/**
 * Use libarchive for tar archives if available falling back to an internal
 * tar implementation.
 */
#ifdef USE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>

/**
 * Fallback internal tar implementation.
 */
#else


#endif

int tar_extract(const char *f, int flags);

#endif
