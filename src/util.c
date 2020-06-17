#include <stdlib.h>

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    p = malloc(n);

    if (!p)
        log_error("Failed to allocate memory");

    return p;
}
