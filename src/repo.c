#include <glob.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "str.h"
#include "vec.h"
#include "util.h"
#include "repo.h"

struct repo *repo_create(void) {
    struct repo *r = malloc(sizeof *r);

    if (r) {
        r->repos = 0;
        r->KISS_PATH = str_init(512);

        if (r->KISS_PATH) {
            return r;
        }

        free(r);
    }

    return NULL;
}

int repo_init(struct repo **r) {
    str_push_s(&(*r)->KISS_PATH, xgetenv("KISS_PATH", ":"));
    str_push_c(&(*r)->KISS_PATH, ':');
    str_push_s(&(*r)->KISS_PATH, xgetenv("KISS_ROOT", "/"));
    str_push_l(&(*r)->KISS_PATH, "var/db/kiss/installed", 21);

    if ((*r)->KISS_PATH->err != STR_OK) {
        err("string error");
        return -1;
    }

    for (char *t = strtok((*r)->KISS_PATH->buf, ":"); t; t = strtok(0, ":")) {
        if (t[0] != '/') {
            err("relative path '%s' found in KISS_PATH", t);
            return -1;
        }

        if (access(t, F_OK) != 0) {
            err("failed to access path '%s': %s", t, strerror(errno));
            return -1;
        }

        vec_push((*r)->repos, path_normalize(t));
    }

    return 0;
}

int repo_find(char **buf, const char *name, char **repos) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        int fd = open(repos[i], O_RDONLY);

        if (fd == -1) {
            err("failed to open repository '%s': %s", 
                repos[i], strerror(errno));
            return -1;
        }

        int err = faccessat(fd, name, F_OK, 0);
        close(fd);

        if (err != -1) {
            *buf = repos[i];
            return 0;

        } else if (errno != ENOENT) {
            err("failed to open pkg '%s/%s': %s", 
                repos[i], name, strerror(errno));
            return -1;
        }
    }

    err("package '%s' not in any repository", name);
    return -2;
}

int repo_glob(glob_t *res, str *buf, const char *query, char **repos) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        str_undo_l(&buf, buf->len);
        str_push_s(&buf, repos[i]);
        str_push_c(&buf, '/');
        str_push_s(&buf, query);
        str_push_c(&buf, '/');

        if (buf->err != STR_OK) {
            err("string error");
            return -1;
        }

        glob(buf->buf, i ? GLOB_APPEND : 0, NULL, res);
    }

    return 0;
}

void repo_free(struct repo **r) {
    str_free(&(*r)->KISS_PATH);
    vec_free((*r)->repos);
    free(*r);
    *r = NULL;
}

