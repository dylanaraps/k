#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "str.h"
#include "vec.h"
#include "util.h"
#include "repo.h"

static str *KISS_PATH = 0;
static str *repo = 0;
static char **repos = 0;

int repo_init(void) {
    KISS_PATH = str_init(512);

    if (!KISS_PATH) {
        return -1;
    }

    repo = str_init(128);

    if (!repo) {
        return -1;
    }

    str_push_s(&KISS_PATH, xgetenv("KISS_PATH", ""));
    str_push_c(&KISS_PATH, ':');
    str_push_s(&KISS_PATH, xgetenv("KISS_ROOT", "/"));
    str_push_l(&KISS_PATH, "var/db/kiss/installed", 21);

    if (KISS_PATH->err != STR_OK) {
        return -1;
    }

    for (char *t = strtok(KISS_PATH->buf, ":"); t; t = strtok(0, ":")) {
        if (t[0] != '/') {
            return -1;
        }

        if (access(t, F_OK) != 0) {
            return -1;
        }

        vec_push(repos, path_normalize(t));
    }

    return 0;
}

int repo_find(char **buf, const char *name) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        str_undo_l(&repo, repo->len);
        str_push_s(&repo, repos[i]);   
        str_push_c(&repo, '/');
        str_push_s(&repo, name);
        str_push_l(&repo, "/version", 8);

        if (repo->err != STR_OK) {
            return -3;

        } else if (access(repo->buf, F_OK) == 0) {
            *buf = repos[i];
            return 0;

        } else if (errno != ENOENT) {
            return -1;
        }
    }

    return -2;
}

void repo_free(void) {
    str_free(&KISS_PATH);
    str_free(&repo);
    vec_free(repos);
}

