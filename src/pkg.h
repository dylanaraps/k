#include <limits.h>

extern char **REPOS;
extern char PWD[PATH_MAX];
extern char *HOME;
extern char *XDG_CACHE_HOME;
extern char *CAC_DIR, *MAK_DIR, *PKG_DIR, *TAR_DIR, *SRC_DIR, *LOG_DIR, *BIN_DIR;

struct version {
    char *version;
    char *release;
};

struct source {
    char *url;
    char *dest;
};

typedef struct package {
    char *name;
    char **path;

    struct source  source;
    struct version version;

    struct package *next;
    struct package *prev;
} package;

void pkg_load(package **head, char *pkg_name);
char **pkg_find(char *pkg_name);
struct version pkg_version(char *repo_dir);
void pkg_sources(struct package pkg);
void cache_init(void);
