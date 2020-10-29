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
    if (path && path[0]) {
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
    }

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

char *repo_find(const char *name, struct repo *repos) {
    for (size_t i = 0; i < vec_size(repos->fds); i++) {
        if (faccessat(repos->fds[i], name, F_OK, 0) != -1) {
            return repos->list[i];

        } else if (errno != ENOENT) {
            err("failed to open pkg '%s/%s': %s", 
                repos->list[i], name, strerror(errno));
            return NULL;
        }
    }

    err("package '%s' not in any repository", name);
    return NULL;
}

static int globat(const char *pwd, const char *query, int opt, glob_t *res) {
    char buf[512];

    if ((strlen(pwd) + strlen(query) + 2) >= sizeof buf) {
        err("buffer overflow");
        return -1;
    }

    strcpy(buf, pwd);
    strcat(buf, "/");
    strcat(buf, query);
    
    switch (glob(buf, opt, NULL, res)) {
        case GLOB_NOSPACE:
        case GLOB_ABORTED:
            err("glob error");
            return -1;

        default:
            return 0;
    }
}

int repo_glob(glob_t *res, const char *query, char **repos) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        if (globat(repos[i], query, i ? GLOB_APPEND : 0, res) < 0) {
            return -1; 
        }
    }

    return 0;
}

char *repo_get_db(void) {
    char *env = getenv("KISS_ROOT");

    if (!env || !env[0]) {
        return strndup(DB_DIR, sizeof (DB_DIR));
    }

    if (env[0] != '/') {
        err("KISS_ROOT not absolute");
        return NULL;
    }

    size_t len = strlen(env);

    while (env[len - 1] == '/') {
        len--;
    }

    char *db = malloc(len + sizeof (DB_DIR));

    if (!db) {
        err("failed to allocate memory");
        return NULL;
    }

    memcpy(db, env, len);
    memcpy(db + len, DB_DIR, sizeof (DB_DIR));

    return db;
}

void repo_free(struct repo **r) {
    // Holds memory to list - last element.
    free((*r)->KISS_PATH);

    // Holds memory to last element in list.
    free((*r)->list[vec_size((*r)->list)  - 1]);

    for (size_t i = 0; i < vec_size((*r)->fds); i++) {
        close((*r)->fds[i]);
    }

    vec_free((*r)->fds);
    vec_free((*r)->list);

    free(*r);
    *r = NULL;
}

