#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "arr.h"
#include "buf.h"
#include "error.h"
#include "repo.h"

struct repo *repo_open(const char *path) {
    int n = open(path, O_RDONLY);

    if (n == -1) {
        err_no("failed to open repo '%s'", path);
        return NULL;
    }

    size_t l = strlen(path) + 1;
    struct repo *r = malloc(sizeof *r + l);

    if (!r) {
        return NULL;
    }

    r->fd = n;
    memcpy(r->path, path, l);
    return r;
}

struct repo *repo_open_db(const char *type) {
    buf *db = buf_alloc(0, 128);

    if (!db) {
        return NULL;
    }

    buf_push_s(&db, getenv("KISS_ROOT"));
    buf_rstrip(&db, '/');
    buf_push_l(&db, "/var/db/kiss/", 13);
    buf_push_s(&db, type);

    struct repo *n = repo_open(db);

    buf_free(&db);

    if (!n) {
        return NULL;
    }

    return n;
}

int repo_open_db_push(struct repo **r, const char *type) {
    struct repo *n = repo_open_db(type);

    if (!n) {
        return -1;
    }

    arr_push_b(r, n);
    return 0;
}

int repo_open_PATH(struct repo **r, const char *PATH) {
    if (!PATH || !PATH[0]) {
        goto open_db;
    }

    char *p = strdup(PATH);

    if (!p) {
        return -ENOMEM;
    }

    for (char *t = strtok(p, ":"); t; t = strtok(NULL, ":")) {
        struct repo *n = repo_open(t);

        if (!n) {
            free(p);
            return -1;
        }

        arr_push_b(r, n);
    }

    free(p);

open_db:;
    struct repo *n = repo_open_db("installed");

    if (!n) {
        return -1;
    }

    arr_push_b(r, n);
    return 0;
}

int repo_has_pkg(struct repo *r, const char *pkg) {
    return faccessat(r->fd, pkg, F_OK, 0) == 0 ? 1 : 0;
}

int repo_find_pkg(struct repo **r, const char *pkg) {
    for (size_t i = 0; i < arr_len(r); i++) {
        if (repo_has_pkg(r[i], pkg)) {
            return r[i]->fd;
        }
    }

    return -1;
}

void repo_free(struct repo *r) {
    close(r->fd);
    free(r);
}

void repo_free_all(struct repo **r) {
    for (size_t i = 0; i < arr_len(r); i++) {
        repo_free(r[i]);
    }

    arr_free(r);
}

