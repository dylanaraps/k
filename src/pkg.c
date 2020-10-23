#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include "str.h"
#include "log.h"
#include "repo.h"
#include "pkg.h"

pkg *pkg_init(const char *name) {
    pkg *new = malloc(sizeof *new);

    if (new) {
        new->name = strdup(name);

        if (!new->name) {
            free(new);
            return NULL;
        }

        new->path = 0;
        new->depends = 0;
        new->sources = 0;
        new->version.ver = 0;
        new->version.rel = 0;
    }

    return new;
}

pkg *pkg_init_die(const char *name) {
    pkg *new = pkg_init(name);

    if (!new) {
        die("failed to allocate memory");
    }

    return new;
}

void pkg_free(pkg **p) {
    free((*p)->name);
    free(*p);
    *p = NULL;
}


void pkg_version(str **s, const char *name, const char *repo) {
    str_undo_l(s, (*s)->len);
    str_push_s(s, repo);
    str_push_c(s, '/');
    str_push_s(s, name);
    str_push_l(s, "/version", 8);

    if ((*s)->err == STR_OK) {
        FILE *f = fopen((*s)->buf, "r");

        if (f) {
            str_undo_l(s, (*s)->len);
            str_getline(s, f);
            fclose(f);

            if ((*s)->err == STR_OK && (*s)->len > 0) {
                return;
            }
        }
    }

    (*s)->err = STR_ERROR;
}

void pkg_list_print(str **s, const char *name, const char *repo) {
    pkg_version(s, name, repo);

    if ((*s)->err == STR_OK) {
        printf("%s %s\n", name, (*s)->buf);

    } else {
        die("package '%s' not installed", name);
    }
}

void pkg_list_installed(str **s, const char *repo) {
    struct dirent **list;

    int len = scandir(repo, &list, 0, alphasort);

    if (len > 0) {
        free(list[0]);
        free(list[1]);

        for (int i = 2; i < len; i++) {
            pkg_list_print(s, list[i]->d_name, repo);
            free(list[i]);
        }

        free(list);

    } else {
        die("database not accessible");
    }
}

