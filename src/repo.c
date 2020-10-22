#include <glob.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "util.h"
#include "str.h"
#include "vec.h"
#include "repo.h"

// String holding the value of getenv("KISS_PATH") in addition to
// getenv("KISS_ROOT")/var/db/kiss/installed.
static str *KISS_PATH = 0;

// Vector holding pointers to each individual repository, fields split from
// KISS_PATH. ie, they share the same memory.
static char **repos = 0;

char **repo_init(void) {
    KISS_PATH = str_init_die(512);

    str_push_s(&KISS_PATH, xgetenv("KISS_PATH", ""));
    str_push_c(&KISS_PATH, ':');
    str_push_s(&KISS_PATH, xgetenv("KISS_ROOT", "/"));
    str_push_l(&KISS_PATH, "var/db/kiss/installed", 21);

    if (KISS_PATH->err != STR_OK) {
        die("failed to read KISS_PATH");
    }

    for (char *t = strtok(KISS_PATH->buf, ":"); t; t = strtok(0, ":")) {
        if (t[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        vec_push(repos, path_normalize(t));
    }

    return repos;
}

void repo_find(str **buf, const char *query) {
    for (size_t j = 0; j < vec_size(repos); j++) {
        str_undo_l(buf, (*buf)->len);
        str_push_s(buf, repos[j]);
        str_push_c(buf, '/');
        str_push_s(buf, query);
        str_push_c(buf, '/');

        if ((*buf)->err == STR_OK && access((*buf)->buf, R_OK) == 0) {
            return;
        }
    }

    (*buf)->err = STR_ERROR;
}

void repo_find_all(str **s, const char *query) {
    glob_t buf = {0};

    for (size_t j = 0; j < vec_size(repos); j++) {
        str_undo_l(s, (*s)->len);
        str_push_s(s, repos[j]);
        str_push_c(s, '/');
        str_push_s(s, query);
        str_push_c(s, '/');

        if ((*s)->err != STR_OK) {
            die("failed to construct glob");
        }

        glob((*s)->buf, j ? GLOB_APPEND : 0, 0, &buf);
    }

    for (size_t j = 0; j < buf.gl_pathc; j++) {
        puts(buf.gl_pathv[j]);
    }

    if (buf.gl_pathc == 0) {
        die("no results for '%s'", query);
    }

    globfree(&buf);
}

void repo_free(void) {
    str_free(KISS_PATH);
    vec_free(repos);
}

