#ifndef KISS_REPO_H
#define KISS_REPO_H

#include "str.h"

struct repo {
    str *KISS_PATH;   
    char **repos;
};

struct repo *repo_create(void);
int repo_init(struct repo **r);
int repo_find(char **buf, const char *name, char **repos);
void repo_free(struct repo **r);

#endif
