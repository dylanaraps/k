#ifndef KISS_REPO_H_
#define KISS_REPO_H_

#include <glob.h>

#include "pkg.h"

void repo_init(void);
void repo_free(void);

glob_t repo_glob(const char *);
char *repo_find(const char *);
void repo_find_all(package *);

#endif
