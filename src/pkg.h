#ifndef ARRAY_H_
#define ARRAY_H_

#include <limits.h>

#define PKG_DB "/var/db/kiss/installed/"

extern char **REPOS;
extern int  REPO_LEN;
extern const char *HOME;
extern const char *XDG_CACHE_HOME;
extern char *CAC_DIR, *MAK_DIR, *PKG_DIR, *TAR_DIR, *SRC_DIR, *BIN_DIR;
extern char *PKG;
extern char *OLD_CWD;
extern char old_cwd_buf[PATH_MAX+1];

struct version {
    char *version;
    char *release;
};

struct source {
    char **src;
    char **dest;
};

typedef struct package {
    char *name;
    char **sums;

    char **path;
    int  path_len;

    struct source source;
    struct version version;
    int  src_len;

    struct package *next;
    struct package *prev;
} package;

void pkg_load(package **head, char *pkg_name);
struct version pkg_version(char *repo_dir);
void cache_init(void);
void cache_destroy(void);

#define SAVE_CWD { \
    OLD_CWD = getcwd(old_cwd_buf, sizeof(old_cwd_buf)); \
}

#define LOAD_CWD { \
    xchdir(OLD_CWD); \
    free(OLD_CWD); \
}

#endif
