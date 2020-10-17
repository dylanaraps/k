#ifndef KISS_UTIL_H_
#define KISS_UTIL_H_

#include <sys/stat.h>
#include <stdlib.h>

#include "str.h"

const char *xgetenv(const char *var, const char *fallback);
char *path_normalize(char *s);
void str_mkdir_die(str *s, mode_t m);
int mkdir_p(char *p, const mode_t m);

#endif
