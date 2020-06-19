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
    package *pkg  = malloc(sizeof(package));    
    package *last = *head;

    if (!pkg)
        die("Failed to allocate memory");

    if (!pkg_name)
        die("Package name is NULL");

    pkg->next     = NULL;
    pkg->name     = strdup(pkg_name);
    pkg->sums     = NULL;
    pkg->path     = NULL;
    pkg->src      = NULL;
    pkg->dest     = NULL;
    pkg->src_len  = 0;
    pkg->path_len = 0;

    pkg_find(pkg);
    pkg_version(pkg);

    snprintf(pkg->build,     PATH_MAX,     "%s/build",  pkg->path[0]);
    snprintf(pkg->build_dir, PATH_MAX + 1, "%s/%s", MAK_DIR, pkg->name);
    snprintf(pkg->dest_dir,  PATH_MAX + 1, "%s/%s", PKG_DIR, pkg->name);

    if (!*head) {
        pkg->prev = NULL;
        *head = pkg;
        return;
    }

    while (last->next)
        last = last->next;

    last->next = pkg;
    pkg->prev = last;
}

void pkg_destroy(package *pkg) {
    for (; pkg; pkg = pkg->next) {
        free(pkg->build);
        free(pkg->dest);
        free(pkg->dest_dir);
        free(pkg->name);
        free(pkg->path);
        free(pkg->rel);
        free(pkg->src);
        free(pkg->sums);
        free(pkg->ver);
        free(pkg);
    }

    free(pkg);
}
