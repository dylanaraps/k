#include <glob.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "vec.h"
#include "util.h"
#include "repo.h"

struct repo *repo_create(void) {
    struct repo *r = malloc(sizeof *r);

    if (r) {
        r->list = 0;
        r->fds  = 0;
        r->mem = str_init(512);

        if (r->mem) {
            return r;
        }

        free(r);
    }

    return NULL;
}

int repo_init(struct repo **r, char *path) {
    if (str_printf(&(*r)->mem, "%s:", path) < 0) {
        err("failed to push KISS_PATH");
        return -1;
    }

    if (repo_get_db(&(*r)->mem) < 0) {
        err("string error");
        return -1;
    }

    for (char *t = strtok((*r)->mem, ":"); t; t = strtok(0, ":")) {
        if (repo_add(r, t) < 0) {
            return -1;
        }
    }

    return 0;
}

int repo_add(struct repo **r, char *path) {
    if (path[0] != '/') {
        err("path '%s' is not absolute", path);
        return -1;
    }

    int repo_fd = open(path, O_RDONLY);

    if (repo_fd == -1) {
        err_no("failed to access path '%s'", path);
        return -1;
    }

    vec_push((*r)->list, path);
    vec_push((*r)->fds, repo_fd);

    return 0;
}

char *repo_find(const char *name, struct repo *r) {
    for (size_t i = 0; i < vec_size(r->fds); i++) {
        if (faccessat(r->fds[i], name, F_OK, 0) != -1) {
            return r->list[i];

        } else if (errno != ENOENT) {
            err_no("failed to open pkg '%s/%s'", r->list[i], name);
            return NULL;
        }
    }

    err("package '%s' not in any repository", name);
    return NULL;
}

int repo_glob(glob_t *res, const char *query, struct repo *r) {
    for (size_t i = 0; i < vec_size(r->list); i++) {
        size_t sl = str_get_len(&r->mem);

        if (str_printf(&r->mem, "%s/%s/", r->list[i], query) < 0) {
            err("failed to construct search query");
            return -1;
        }

        switch (glob(r->mem + sl, i ? GLOB_APPEND : 0, NULL, res)) {
            case GLOB_NOSPACE:
            case GLOB_ABORTED:
                err("glob error");
                return -1;
        }

        str_set_len(&r->mem, sl);
    }

    return 0;
}

int repo_get_db(str **buf) {
    // ignore -2 (EINVAL) when KISS_ROOT unset.
    if (str_push_s(buf, getenv("KISS_ROOT")) == -1) {
        err("failed to allocate memory");
        return -1;
    }

    // drop all trailing slashes from KISS_ROOT.
    str_undo_c(buf, '/');

    if (str_push_l(buf, "/var/db/kiss/installed", 22) < 0) {
        err("failed to push database directory");
        return -1;
    }

    return 0;
}

void repo_free(struct repo **r) {
    str_free(&(*r)->mem);

    for (size_t i = 0; i < vec_size((*r)->fds); i++) {
        close((*r)->fds[i]);
    }

    vec_free((*r)->fds);
    vec_free((*r)->list);

    free(*r);
    *r = NULL;
}

