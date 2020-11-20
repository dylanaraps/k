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

/**
 * Open a repository. Returns NULL on error.
 */
struct repo *repo_open(const char *path);

/**
 * Open a $PATH and store each path in the array r. Returns < 0 on error;
 */
int repo_open_PATH(struct repo **r, const char *PATH);

/**
 * Open the installed database of type. Returns NULL on error;
 */
struct repo *repo_open_db(const char *type);

/**
 * Open the installed database of type and push it to the array. Returns -1
 * on error;
 */
int repo_open_db_push(struct repo **r, const char *type);

/**
 * Check if package exists in repository. Returns 1 for success and 0 for
 * error.
 */
int repo_has_pkg(struct repo *r, const char *pkg);
/**
 * Find the first occurrence of pkg in a repository list;
 */
int repo_find_pkg(struct repo **r, const char *pkg);

/**
 * Free all memory associated with the repository and close its fd;
 */
void repo_free(struct repo *r);

/**
 * Free all memory and close any fds associated with a list of repositories.
 * Also frees the array itself.
 */
void repo_free_all(struct repo **r);

#endif
