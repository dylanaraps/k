
extern char **REPOS;

typedef struct package {
    char *name;
    char *version;
    char *release;
    char *repository;

    struct package *next;
    struct package *prev;
} package;

void pkg_load(package **head, char *pkg_name);
char *pkg_find(char *pkg_name, char **repos);
