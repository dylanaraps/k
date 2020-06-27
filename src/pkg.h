typedef struct package {
    char *name;
    char *ver;
    char *path;

    char **sum;
    char **src;
    char **des;
} package;

#define pkg_iter(p) for (size_t i = 0; i < vec_size(p); ++i)
