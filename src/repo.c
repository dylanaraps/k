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
        r->list = 0;
        r->fds  = 0;
        r->KISS_PATH = 0;
    }

    return r;
}

int repo_init(struct repo **r, char *path) {
    if (!path || !path[0]) {
        goto db;
    }

    (*r)->KISS_PATH = strdup(path);

    if (!(*r)->KISS_PATH) {
        err("failed to allocate memory");
        return -1; 
    }

    for (char *t = strtok((*r)->KISS_PATH, ":"); t; t = strtok(0, ":")) {
        if (repo_add(r, t) < 0) {
            return -1;
        }
    }

db:;
    char *db = repo_get_db();

    if (!db) {
        return -1;
    }

    return repo_add(r, db);
}

int repo_add(struct repo **r, char *path) {
    if (path[0] != '/') {
        err("relative path '%s' found in KISS_PATH", path);
        return -1;
    }

    int repo_fd = open(path, O_RDONLY);

    if (repo_fd == -1) {
        err("failed to access path '%s': %s", path, strerror(errno));
        return -1;
    }

    vec_push((*r)->list, path_normalize(path));
    vec_push((*r)->fds, repo_fd);

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

int repo_glob(glob_t *res, const char *query, char **repos) {
    str *full_query = str_init(128);

    if (!full_query) {
        err("failed to allocate memory");
        return -1;
    }

    for (size_t i = 0; i < vec_size(repos); i++) {
        str_undo_l(&full_query, full_query->len);
        str_push_s(&full_query, repos[i]);
        str_push_c(&full_query, '/');
        str_push_s(&full_query, query);
        str_push_c(&full_query, '/');

        if (full_query->err != STR_OK) {
            str_free(&full_query);
            err("string error");
            return -1;
        }

        glob(full_query->buf, i ? GLOB_APPEND : 0, NULL, res);
    }

    str_free(&full_query);
    return 0;
}

char *repo_get_db(void) {
    char *env = getenv("KISS_ROOT");

    if (!env || !env[0]) {
        return strdup(DB_DIR);
    }

    if (env[0] != '/') {
        err("KISS_ROOT not absolute");
        return NULL;
    }

    size_t len = strlen(env);

    while (env[len - 1] == '/') {
        len--;
    }

    char *db = malloc(len + sizeof(DB_DIR));

    if (!db) {
        err("failed to allocate memory");
        return NULL;
    }

    memcpy(db, env, len);
    memcpy(db + len, DB_DIR, sizeof (DB_DIR));

    return db;
}

void repo_free(struct repo **r) {
    free((*r)->list[vec_size((*r)->list)  - 1]);
    vec_free((*r)->list);

    for (size_t i = 0; i < vec_size((*r)->fds); i++) {
        close((*r)->fds[i]);
    }

    vec_free((*r)->fds);

    free((*r)->KISS_PATH);
    free(*r);
    *r = NULL;
}

