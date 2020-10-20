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

char *path_normalize(char *d) {
    if (d) {
        for (size_t i = 1, l = strlen(d);
             d[l - i] == '/';
             d[l - i] = 0, i++);
    }

    return d;
}

static int _rm_rf(const char *p, const struct stat *b, int t, struct FTW *f) {
    (void) b;
    (void) t;
    (void) f;

    return remove(p);
}

int rm_rf(const char *d) {
    return nftw(d, _rm_rf, 512, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
}

