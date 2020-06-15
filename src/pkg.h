
extern char **REPOS;

struct version {
    char *version;
    char *release;
};

struct source {
    char *url;
    char *file;
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
char **pkg_find(char *pkg_name, char **repos);
struct version pkg_version(char *pkg_name, char *repo);
