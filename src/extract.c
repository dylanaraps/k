#include <stdlib.h>
#include <stdio.h>

#include "extract.h"
#include "pkg.h"

void pkg_extract(package *pkg) {
    int i;

    if (pkg->src_len == 0) {
        printf("error: no sources\n");
        exit(1);
    }

    for (i = 0; i < pkg->src_len; i++) {

    }
}

