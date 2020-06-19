#include <stdlib.h> /* malloc, size_t */

#include "log.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    if (n == 0) {
        die("Empty memory allocation");    
    }

    p = malloc(n);

    if (!p) {
        die("Failed to allocate memory");
    }

    msg("Allocated %zu bytes", n);

    return p;
}


int cntchr(const char *str, int chr) {
    const char *tmp = str;
    int i = 0;

    for (; tmp[i]; tmp[i] == chr ? i++ : *tmp++);   

    return i;
}
