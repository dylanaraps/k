#ifndef ARRAY_H_
#define ARRAY_H_

#include <limits.h>

#include "util.h"

/* for PATH_MAX on systems that don't have it in limits.h */
#include <sys/param.h>
#ifndef PATH_MAX
#define  PATH_MAX         256
#endif

#define PKG_DB "/var/db/kiss/installed/"

extern char **REPOS;
extern int  REPO_LEN;
extern char *PKG;
extern char HOME[PATH_MAX];
extern char XDG_CACHE_HOME[PATH_MAX];
extern char CAC_DIR[PATH_MAX];
extern char SRC_DIR[PATH_MAX];
extern char BIN_DIR[PATH_MAX];

extern char MAK_DIR[PATH_MAX];
extern char PKG_DIR[PATH_MAX];
extern char TAR_DIR[PATH_MAX];

extern char *OLD_CWD;
extern char old_cwd_buf[PATH_MAX];

typedef struct package {
    char *name;
    char *ver;
    char *rel;
    char build[PATH_MAX];
    char build_dir[PATH_MAX + 1];
    char dest_dir[PATH_MAX + 1];
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

#endif
