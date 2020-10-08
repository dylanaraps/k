#ifndef KISS_REPO_H_
#define KISS_REPO_H_

#include <glob.h>

#include "pkg.h"

char **repo_init(void);
void repo_free(char **);
glob_t repo_glob(const char *, char **);
char *repo_find(const char *, char **);
void repo_find_all(package *, char **);

#endif
