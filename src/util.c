#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    p = malloc(n);

    if (!p)
        log_error("Failed to allocate memory");

    return p;
}

void xchdir(const char *path) {
    int ret;

    if (!path)
        log_error("Directory %s not accessible", path);

    ret = chdir(path);
    
    if (ret != 0)
        log_error("Directory %s not accessible", path);
}
