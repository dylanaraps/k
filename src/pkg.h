typedef struct package {
    char *name;    

    struct package *next;
    struct package *prev;
} package;

void pkg_init(package **pkg, char *pkg_name);
