#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <ftw.h>

#include "str.h"
#include "log.h"
#include "file.h"

char *path_normalize(char *d) {
    if (d) {
        for (size_t i = 1, l = strlen(d);
             d[l - i] == '/';
             d[l - i] = 0, i++);
    }

    return d;
}

void mkdir_die(str *s) {
    if (s->err != STR_OK) {
       return;
    }

    if (mkdir(s->buf, 0755) == -1) {
        switch (errno) {
            case EEXIST:
                break;

            case ENOENT:
                die("directory component does not exist '%s'", s->buf);

            default:
                die("failed to create directory '%s'", s->buf);
        }
    }
}

static int _rm_rf(const char *p, const struct stat *b, int t, struct FTW *f) {
    (void) b;
    (void) t;
    (void) f;

    return remove(p);
}

int rm_rf(const char *d) {
    return nftw(d, _rm_rf, 512, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
}

