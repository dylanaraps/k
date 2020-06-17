#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pkg.h"
#include "build.h"

void pkg_build(package *pkg) {
    if (chdir(MAK_DIR) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }

    if (chdir(pkg->name) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }
}
