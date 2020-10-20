#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <sys/stat.h>
#include <stdlib.h>

#include "str.h"

const char *xgetenv(const char *var, const char *fallback);
char *path_normalize(char *d);
int rm_rf(const char *d);

#endif
