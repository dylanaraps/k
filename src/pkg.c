#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include "str.h"
#include "log.h"
#include "repo.h"
#include "vec.h"
#include "pkg.h"

pkg *pkg_create(void) {
    pkg *new = malloc(sizeof *new);

    if (!new) {
        die("failed to allocate memory");
    }

    new->name    = str_init_die(0);
    new->repo    = str_init_die(0);
    new->version = str_init_die(0);
    new->source  = 0;
    new->src_off = 0;
    new->depend  = 0;
    new->dep_off = 0;

    return new;
}

pkg *pkg_init(const char *name) {
    pkg *new = pkg_create();

    pkg_init_name(&new, name);
    pkg_init_repo(&new, NULL);
    pkg_init_version(&new);
    pkg_init_sources(&new);
    pkg_init_depends(&new);

    return new;
}

void pkg_init_name(pkg **p, const char *name) {
    str_push_s(&(*p)->name, name);

    if ((*p)->name->err != STR_OK) {
        die("failed to initialize package %s", name);
    }
}

void pkg_init_repo(pkg **p, const char *repo) {
    if (repo) {
        str_push_s(&(*p)->repo, repo);

    } else {
        repo_find(&(*p)->repo, (*p)->name->buf);
    }

    if ((*p)->repo->err != STR_OK) {
        die("failed to locate package %s", (*p)->name->buf);
    }
}

void pkg_init_sources(pkg **p) {
    FILE *f = pkg_fopen(p, "sources", 7);

    if (!f) {
        msg("[%s] no sources file, skipping", (*p)->name->buf);
        return;
    }

    str *tmp = str_init_die(0);

    while (tmp->err != STR_EOF) {
        str_undo_l(&tmp, tmp->len);

        if (str_getline(&tmp, f) == 0) {
            continue;
        }

        if (tmp->buf[0] == '#') {
            continue;
        }

        vec_push((*p)->src_off, str_rchr(tmp, ' '));
        vec_push((*p)->source, str_dup_die(&tmp));
    }

    fclose(f);
    str_free(&tmp);
}

void pkg_init_depends(pkg **p) {
    FILE *f = pkg_fopen(p, "depends", 7);

    if (!f) {
        msg("[%s] no depends file, skipping", (*p)->name->buf);
        return;
    }

    str *tmp = str_init_die(24);

    while (tmp->err != STR_EOF) {
        str_undo_l(&tmp, tmp->len);

        if (str_getline(&tmp, f) == 0) {
            continue;
        }

        if (tmp->buf[0] == '#') {
            continue;
        }

        vec_push((*p)->dep_off, str_rchr(tmp, ' '));
        vec_push((*p)->depend, str_dup_die(&tmp));
    }

    fclose(f);
    str_free(&tmp);
}

int pkg_init_version(pkg **p) {
    FILE *f = pkg_fopen(p, "version", 7);

    if (!f) {
        return -1;
    }

    str_getline(&(*p)->version, f);
    fclose(f);

    if ((*p)->version->err != STR_OK || 
        (*p)->version->len == 0) {
        return -2;
    }

    return 0;
}

FILE *pkg_fopen(pkg **p, const char *file, size_t len) {
    str_push_l(&(*p)->repo, file, len);
    FILE *f = fopen((*p)->repo->buf, "r");
    str_undo_l(&(*p)->repo, len);
    return f;
}

void pkg_free(pkg **p) {
    str_free(&(*p)->name);
    str_free(&(*p)->repo);
    str_free(&(*p)->version);

    vec_free((*p)->src_off);
    vec_free((*p)->dep_off);

    for (size_t i = 0; i < vec_size((*p)->source); i++) {
        free((*p)->source[i]);
    }
    vec_free((*p)->source);

    for (size_t i = 0; i < vec_size((*p)->depend); i++) {
        str_free(&(*p)->depend[i]);
    }
    vec_free((*p)->depend);

    free(*p);
    *p = NULL;
}

void pkg_list_print(str **s, const char *name, const char *repo) {
    str_undo_l(s, (*s)->len);
    str_push_s(s, repo);
    str_push_c(s, '/');
    str_push_s(s, name);
    str_push_l(s, "/version", 8);

    if ((*s)->err != STR_OK) {
        die("failed to construct string");
    }

    FILE *f = fopen((*s)->buf, "r");

    if (!f) {
        die("package %s not installed", name);
    }

    str_undo_l(s, (*s)->len);
    str_getline(s, f);
    fclose(f);

    if ((*s)->err != STR_OK) {
        die("[%s] version file empty", name);
    }

    printf("%s %s\n", name, (*s)->buf);
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

