#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <stdint.h>
#include <sys/stat.h>

#include "find.h"
#include "log.h"
#include "util.h"
#include "version.h"
#include "pkg.h"

void pkg_init(package **head, char *pkg_name) {
    package *new_pkg = malloc(sizeof(package));    
    package *last = *head;

    if (!new_pkg)
        log_error("Failed to allocate memory");

    new_pkg->next     = NULL;
    new_pkg->name     = strdup(pkg_name);
    new_pkg->sums     = NULL;
    new_pkg->path     = NULL;
    new_pkg->src      = NULL;
    new_pkg->dest     = NULL;
    new_pkg->src_len  = 0;
    new_pkg->path_len = 0;

    pkg_find(new_pkg);
    pkg_version(new_pkg);

    if (!*head) {
        new_pkg->prev = NULL;
        *head = new_pkg;
        return;
    }

    while (last->next)
        last = last->next;

    last->next = new_pkg;
    new_pkg->prev = last;
}

void pkg_destroy(package *pkg) {
    for (; pkg; pkg = pkg->next) {
        free(pkg->name);
        free(pkg->sums);
        free(pkg->path);
        free(pkg->src);
        free(pkg->dest);
        free(pkg->ver);
        free(pkg->rel);
        free(pkg);
    }

    free(pkg);
}


