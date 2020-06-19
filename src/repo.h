typedef struct repo {
    char *name;    

    struct repo *prev;
    struct repo *next;
} repo;

void repo_init(void);
