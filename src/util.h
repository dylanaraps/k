char **get_repos(void);
char *xgetenv(const char *);
char *xgetcwd(void);
char *xstrdup(const char *);
char *path_join(const char *, const char *);
int is_dir(const char *d);

#define msg(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        puts(""); \
    } while (0)

#define die(...) \
    do { \
        msg(__VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } while (0)
