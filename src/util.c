#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <glob.h>
#include <sys/stat.h>

#include "util.h"
#include "vec.h"

char **get_repos(void) {
    char **repos = NULL;
    char *p = NULL;
    char *tok;
    char *path = xgetenv("KISS_PATH");

    for (tok = strtok_r(path, ":", &p);
         tok != NULL;
         tok = strtok_r(NULL, ":", &p)) {
        vec_append(repos, xstrdup(tok));
    }

    free(path);
    vec_append(repos, "/var/db/kiss/installed");

    return repos;
}

char *path_join(const char *dir, const char *file) {
    int len = snprintf(NULL, 0, "%s/%s", dir, file) + 1;

    if (len < 0) {
        perror("snprintf");
        exit(1);
    }

    char *buf = malloc(len);

    if (!buf) {
        perror("malloc");
        exit(1);
    }

    int ret = snprintf(buf, len, "%s/%s", dir, file);

    if (ret < 0) {
        perror("snprintf");
        exit(1);
    }

    return buf;
}

/* char *pkg_find(const char *pattern) { */
/*     glob_t buf; */

/*     for (size_t i = 0; i < vec_size(g_repos); ++i) { */
/*         size_t len = strlen(g_repos[i]) + strlen(pattern) + 2; */
/*         char *query = malloc(len); */

/*         if (!query) { */
/*             perror("malloc"); */
/*             exit(1); */
/*         } */

/*         int err = snprintf(query, len, "%s/%s" */

/*         int err = glob(query, GLOB_NOSORT, NULL, &buf); */

/*         if (err != 0) { */
/*             die("Package not found"); */
/*         } */

/*         globfree(&buf); */
/*     } */
/*     return buf.gl_pathv[0]; */
/* } */

char *xgetenv(const char *s) {
    char *p = getenv(s);

    if (!p && !p[0]) {
        return NULL;
    }

    char *p2 = strdup(p);

    if (!p2) {
        perror("strdup");
        exit(1);
    }

    return p2;
}

char *xgetcwd(void) {
    long len = pathconf(".", _PC_PATH_MAX);

    if (len < 0) {
        perror("pathconf");
        exit(1);
    }

    char *buf = malloc((size_t) len);

    if (!buf) {
        perror("malloc");
        exit(1);
    }

    char *cwd = getcwd(buf, (size_t) len);

    if (!cwd) {
        perror("getcwd");
        exit(1);
    }

    return cwd;
}

char *xstrdup(const char *s) {
    if (!s) {
        return NULL;
    }

    char *p = strdup(s);    

    if (!p) {
        return NULL;
    }

    return p;
}

int is_dir(const char *d) {
    struct stat s;

    int err = lstat(d, &s);

    if (err < 0) {
        return 1;
    }

    if (S_ISDIR(s.st_mode)) {
        return 0;
    }

    return 1;
}
