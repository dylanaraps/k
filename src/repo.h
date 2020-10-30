#ifndef KISS_REPO_H
#define KISS_REPO_H

#include "str.h"

struct repo {
    str *KISS_PATH;   
    char **list;
    int *fds;
};

struct repo *repo_create(void);
int repo_add(struct repo **r, char *path);
int repo_init(struct repo **r, char *path);
char *repo_find(const char *name, struct repo *repos);
int repo_glob(glob_t *res, const char *query, char **repos);
void repo_free(struct repo **r);

#define DB_DIR "/var/db/kiss/installed"

#endif
