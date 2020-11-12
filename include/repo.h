/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_REPO_H
#define KISS_REPO_H

struct repo {
    int fd;
    char path[];
};

struct repo *repo_open(const char *path);
int repo_open_PATH(struct repo **r, const char *PATH);
struct repo *repo_open_db(void);
int repo_find_pkg(struct repo **r, const char *pkg);
void repo_free(struct repo *r);
void repo_free_all(struct repo **r);

#endif
