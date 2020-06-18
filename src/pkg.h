#ifndef ARRAY_H_
#define ARRAY_H_

#include <limits.h>

#include "util.h"

#define PKG_DB "/var/db/kiss/installed/"

extern char **REPOS;
extern int  REPO_LEN;
extern char *PKG;
extern char HOME[PATH_MAX + 1];
extern char XDG_CACHE_HOME[PATH_MAX + 1];
extern char CAC_DIR[PATH_MAX + 1];
extern char SRC_DIR[PATH_MAX + 1];
extern char BIN_DIR[PATH_MAX + 1];

extern char MAK_DIR[PATH_MAX + 22];
extern char PKG_DIR[PATH_MAX + 22];
extern char TAR_DIR[PATH_MAX + 22];

extern char *OLD_CWD;
extern char old_cwd_buf[PATH_MAX+1];

typedef struct package {
    char *name;
    char *ver;
    char *rel;
    char **sums;

    char **path;
    int  path_len;

    char **src;
    char **dest;
    int  src_len;

    struct package *next;
    struct package *prev;
} package;

void pkg_init(package **head, char *pkg_name);
void pkg_destroy(package *pkg);

#define SAVE_CWD { \
    OLD_CWD = getcwd(old_cwd_buf, sizeof(old_cwd_buf)); \
}

#define LOAD_CWD { \
    xchdir(OLD_CWD); \
    free(OLD_CWD); \
}

#define do1(f) { \
for (package *tmp = head; tmp; tmp = tmp->next) { \
    PKG = tmp->name; \
    (*f)(tmp); \
} } \

#define do2(f, f2) { \
for (package *tmp = head; tmp; tmp = tmp->next) { \
    PKG = tmp->name; \
    (*f)(tmp); \
    (*f2)(tmp); \
} } \

/* for PATH_MAX on systems that don't have it in limits.h */
#include <sys/param.h>
#ifndef PATH_MAX
#define  PATH_MAX         256
#endif

#endif
