#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "str.h"
#include "util.h"

int mkdir_p(char *p, const mode_t m) {
    for (char *d = p + 1; *d; d++) {
        if (*d == '/') {
            *d = 0;

            if (mkdir(p, m) == -1 && errno != EEXIST) {
                return 1;
            }

            *d = '/';
        }
    }

    return 0;
}

int is_dir(const char *d) {
    struct stat s;

    if (lstat(d, &s) < 0) {
        return 1;
    }

    if (S_ISDIR(s.st_mode)) {
        return 0;
    }

    return 1;
}

int rm_rf(const char *p) {
    DIR *dir = opendir(p);

    if (!dir) {
        msg("opendir failed %s\n", p);
        return 1;
    }

    str *tmp = NULL;
    str_push(&tmp, p);

    if (tmp->buf[tmp->len - 1] != '/') {
        str_push(&tmp, "/");
    }

    struct dirent *e;

    while ((e = readdir(dir))) {
        if ((!e->d_name[1] && e->d_name[0] == '.') ||
            (!e->d_name[2] && e->d_name[0] == '.' && e->d_name[1] == '.')) {
            continue;
        }

        str_push(&tmp, e->d_name);

        if (is_dir(tmp->buf) == 0) {
            if (rm_rf(tmp->buf) != 0) {
                msg("failed to open directory %s", tmp->buf);
                break;
            }

        } else {
            if (unlink(tmp->buf) != 0) {
                msg("failed to remove file %s", tmp->buf);
            }
        }

        str_undo(&tmp, e->d_name);
    }

    str_free(&tmp);
    closedir(dir);

    if (rmdir(p) != 0) {
        msg("failed to remove directory %s", p);
    }

    return 0;
}
