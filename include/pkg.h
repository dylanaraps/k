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
FILE *pkg_fopen(pkg *p, const char *f, int M, const char *m);
int pkg_faccessat(int repo_fd, const char *pkg, const char *file);
int pkg_sort_name(void const *a, void const *b);

enum pkg_source_types {
    SRC_URL,
    SRC_GIT,
    SRC_ABS,
    SRC_REL,
};
int pkg_source_type(pkg *p, char *src);

#endif
