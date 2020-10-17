#include <stdlib.h>
#include <string.h>

#include "util.h"

const char *xgetenv(const char *var, const char *fallback) {
    char *env = getenv(var);

    return env ? env : fallback;
}

char *path_normalize(char *s) {
    if (s) {
        for (
            size_t i = 1, l = strlen(s);
            s[l - i] == '/';
            s[l - i] = 0, i++
        );
    }

    return s;
}

