#include <limits.h>

extern char **REPOS;
extern char PWD[PATH_MAX];

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
void pkg_list(char *pkg_name);
