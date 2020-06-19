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

int cntchr(char *str, int chr) {
    char *tmp = str;
    int i = 0;

    for (; tmp[i]; tmp[i] == chr ? i++ : *tmp++);   

    return i;
}
