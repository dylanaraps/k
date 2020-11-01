#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "str.h"
#include "repo.h"

struct repo *repo_create(void) {
    struct repo *r = malloc(sizeof *r);

    if (r) {
        r->fd   = 0;
        r->path = str_init(256);

        if (r->path) {
            return r;
        }

        free(r);
    }

    return NULL;
}

int repo_init(struct repo **r, const char *path) {
    if (str_push_s(&(*r)->path, path) < 0) {
        return -1;
    }

    if (((*r)->fd = open((*r)->path, O_RDONLY)) < 0) {
        return -1;
    }

    return 0;
}

void repo_free(struct repo **r) {
    str_free(&(*r)->path);
    close((*r)->fd);
    free(*r);
    *r = 0;
}

