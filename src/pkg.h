typedef struct package {
    char *name;
    char *ver[2];
    char *path;

    char **sum;
    char **src;
    char **des;
} package;
