typedef struct package {
    char *name;    
    char *junk;

    struct package *next;
    struct package *prev;
} package;

void pkg_init(package **pkg, char *pkg_name);
