#include <stdio.h>

#include "file.h"

int copy_stream(FILE *r, FILE *w) {
    char buf[BUFSIZ];

    for (unsigned long n = 0 ; !feof(r) ;) {
        n = fread(buf, 1, sizeof buf, r);

        if (ferror(r)) {
            return -1;
        }

        fwrite(buf, 1, n, w);

        if (ferror(w)) {
            return -1;
        }
    }

    return 0;
}

int copy_file(const char *s, const char *d) {
    FILE *r = fopen(s, "r");

    if (!r) {
        return -1;
    }

    FILE *w = fopen(d, "w");

    if (!w) {
        fclose(r);
        return -1;
    }

    int ret = copy_stream(r, w);

    fclose(r);
    fclose(w);

    return ret;    
}

