#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <glob.h>

#include "util.h"
#include "vec.h"
#include "pkg.h"
#include "str.h"
#include "repo.h"

static char **repos = NULL;

void repo_init(void) {
    char *p = NULL;

    str path = {0};
    str_cat(&path, getenv("KISS_PATH"));

    for (char *tok = strtok_r(path.buf, ":", &p);
         tok != NULL;
         tok = strtok_r(NULL, ":", &p)) {

        if (tok[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        vec_add(repos, strdup(tok));
    }
    free(p);
    str_free(&path);

    vec_add(repos, strdup(DB_DIR));
}

void repo_free(void) {
    if (repos) {
        for (size_t i = 0; i < vec_size(repos); i++) {
            free(repos[i]);
        }
        vec_free(repos);
    }
}

glob_t repo_glob(const char *pattern) {
    glob_t buf;

    if (vec_size(repos) == 0) {
        repo_init();
    }

    for (size_t i = 0; i < vec_size(repos); ++i) {
        str query = {0};

        str_cat(&query, repos[i]);
        str_cat(&query, "/");
        str_cat(&query, pattern);
        str_cat(&query, "/");

        glob(query.buf, i ? GLOB_APPEND : 0, NULL, &buf);

        str_free(&query);
    }

    return buf;
}

char *repo_find(const char *pattern) {
    glob_t buf = repo_glob(pattern);

    char *match = NULL;

    if (buf.gl_pathc != 0 && buf.gl_pathv[0]) {
        match = strdup(buf.gl_pathv[0]);
    }

    globfree(&buf);
    return match;
}

void repo_find_all(package *pkgs) {
    for (size_t i = 0; i < vec_size(pkgs); ++i) {
        glob_t buf = repo_glob(pkgs[i].name);

        if (buf.gl_pathc == 0) {
            die("no results for '%s'", pkgs[i].name);
        }

        for (size_t i = 0; i < buf.gl_pathc; i++) {
            puts(buf.gl_pathv[i]);
        }

        globfree(&buf);
    }
}
