#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <stdlib.h>

const char *xgetenv(const char *var, const char *fallback);
char *path_normalize(char *s);

#endif
