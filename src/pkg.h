typedef struct package {
    char *name;
    char *ver[2];

    /* index to repository */
    int  path;

    char **sum;
    char **src;
    char **des;
} package;
