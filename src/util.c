#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ftw.h>

#include "log.h"
#include "str.h"
#include "util.h"

const char *xgetenv(const char *var, const char *fallback) {
    char *env = getenv(var);

    return env ? env : fallback;
}

