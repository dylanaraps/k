#include <stdlib.h> /* malloc, size_t */

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    p = malloc(n);

    if (!p)
        die("Failed to allocate memory");

    return p;
}
