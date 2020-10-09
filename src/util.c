#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "str.h"
#include "util.h"

int PATH_prepend(const char *path, const char *var) {
    if (!path || !var) {
        return 1;
    }

    str kiss_path = {0};

    str_push(&kiss_path, path);
    str_push(&kiss_path, ":");
    str_push(&kiss_path, getenv(var));

    int err = setenv(var, kiss_path.buf, 1);
    free(kiss_path.buf);

    if (err == -1) {
        perror("setenv");
        return 1;
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

char *path_basename(char *p) {
    if (!p) {
        return NULL;
    }

    size_t len = strlen(p);

    for (size_t i = 1; p[len - i] == '/'; i++) {
        p[len - i] = 0;
    }

    char *b = strrchr(p, '/');

    if (!b) {
        *p = '/';
        return p;
    }

    *b++ = 0;

    return b; // p now points to dirname
}

FILE *fopenat(const char *d, const char *f, const char *m) {
    if (!d || !f || !m) {
        return NULL;
    }

    str new = {0};
    str_push(&new, d);
    str_push(&new, "/");
    str_push(&new, f);

    FILE *f2 = fopen(new.buf, m);

    free(new.buf);

    if (!f2) {
        return NULL;
    }

    return f2;
}

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

    return mkdir(p, m) == -1 && errno != EEXIST;
}

int mkdir_e(char *p, const mode_t m) {
    return mkdir(p, m) == -1 && errno != EEXIST;
}

char *pid_to_str(pid_t p) {
    int len = snprintf(NULL, 0, "%u", p);

    if (len < 0) {
        return NULL;
    }

    char *pid = malloc((size_t) len + 1);

    if (!pid) {
        perror("malloc");
        exit(1);
    }

    int err = snprintf(pid, (size_t) len + 1, "%u", p);

    if (err != len) {
        free(pid);
        return NULL;
    }

    return pid;
}
