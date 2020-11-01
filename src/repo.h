#ifndef KISS_REPO_H
#define KISS_REPO_H

#include "str.h"

struct repo {
    str *path;
    int fd;
};

struct repo *repo_create(void);
int repo_init(struct repo **r, const char *path);
void repo_free(struct repo **r);

#endif
