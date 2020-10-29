#ifndef KISS_REPO_H
#define KISS_REPO_H

#include "str.h"

struct repo {
    char *KISS_PATH;   
    char **list;
    int *fds;
};

struct repo *repo_create(void);
int repo_add(struct repo **r, char *path);
int repo_init(struct repo **r, char *path);
int repo_find(char **buf, const char *name, char **repos);
int repo_glob(glob_t *res, const char *query, char **repos);
char *repo_get_db(void);
void repo_free(struct repo **r);

#define DB_DIR "/var/db/kiss/installed"

#endif
