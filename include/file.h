/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_FILE_H
#define KISS_FILE_H

#include <sys/stat.h>

/**
 * Recursively create a directory tree. The last path component is skipped
 * unless the path ends in '/'. Returns < 0 for error.
 */
int mkdir_p(const char *path, mode_t m);

/**
 * Recursively remove a directory tree. Returns < 0 for error.
 */
int rm_rf(const char *path);

/**
 * Wrapper around openat() to return a FILE rather than an fd.
 */
FILE *fopenat(int fd, const char *path, int m, const char *m2);

#endif
