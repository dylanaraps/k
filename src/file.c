#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ftw.h>

#include "error.h"
#include "file.h"

int mkdir_p(const char *path, mode_t m) {
    for (char *p = strchr(path + 1, '/'); p; p = strchr(p + 1, '/')) {
        *p = 0;
        int ret = mkdir(path, m);
        *p = '/';

        if (ret == -1 && errno != EEXIST) {
            err_no("failed to create directory '%s'", path);
            return -1;
        }
    }

    return 0;
}

static int _rm_rf(const char *p, const struct stat *sb, int t, struct FTW *b) {
    (void) sb;
    (void) t;
    (void) b;

    if (remove(p) == -1) {
        err_no("failed to remove file '%s'", p);
        return -1;
    }

    return 0;
}

int rm_rf(const char *path) {
    return nftw(path, _rm_rf, 64, FTW_DEPTH | FTW_PHYS);
}

FILE *fopenat(int fd, const char *p, int m, const char *M) {
    int fd2 = openat(fd, p, m);

    if (fd2 == -1) {
        return NULL;
    }

    return fdopen(fd2, M);
}

FILE *fopenatat(int fd, const char *p, const char *f, int m, const char *M) {
    int pfd = openat(fd, p, O_RDONLY);

    if (pfd == -1) {
        return NULL;
    }

    int ffd = openat(pfd, f, m, 0644);
    close(pfd);

    if (ffd == -1) {
        return NULL;
    }

    return fdopen(ffd, M);
}

