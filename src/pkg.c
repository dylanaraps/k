#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "pkg.h"

FILE *pkg_fopen(int repo_fd, const char *pkg, const char *file) {
    int pfd = openat(repo_fd, pkg, O_RDONLY);

    if (pfd == -1) {
        return NULL;
    }

    int ffd = openat(pfd, file, O_RDONLY);
    close(pfd);

    if (ffd == -1) {
        return NULL;
    }

    return fdopen(ffd, "r");
}
