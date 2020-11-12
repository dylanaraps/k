/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_PKG_H
#define KISS_PKG_H

typedef struct {
   char *name;
   int repo_fd;
} pkg;

pkg *pkg_alloc(const char *name);
int pkg_find_path(pkg *p);
void pkg_free(pkg *p);
void pkg_free_all(pkg **p);
FILE *pkg_fopen(int repo_fd, const char *pkg, const char *file);

#endif
