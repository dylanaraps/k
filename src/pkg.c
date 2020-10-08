#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "util.h"
#include "repo.h"
#include "vec.h"
#include "str.h"
#include "pkg.h"

package pkg_init(const char *name) {
    package pkg = {0};

    pkg.name = strdup(name);
    pkg.path = repo_find(name);

    return pkg;
}

void pkg_free(package *pkgs) {
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        free(pkgs[i].name);
        free(pkgs[i].path);
    }
    vec_free(pkgs);
}

int pkg_list(const char *name) {
    str p = {0};

    str_cat(&p, DB_DIR);
    str_cat(&p, "/");
    str_cat(&p, name);

    int ret = is_dir(p.buf);

    str_free(&p);

    return !ret;
}

void pkg_list_all(package *pkgs) {
    if (vec_size(pkgs) == 0) {
        struct dirent **list;
        int len = scandir(DB_DIR, &list, NULL, alphasort);

        if (len == -1) {
            die("database not accessible");
        }

        for (int i = 2; i < len; i++) {
            vec_add(pkgs, pkg_init(list[i]->d_name));
            free(list[i]);
        }
        free(list);
    }

    for (size_t i = 0; i < vec_size(pkgs); ++i) {
        if (!pkg_list(pkgs[i].name)) {
            die("package '%s' not installed", pkgs[i].name);
        }

        puts(pkgs[i].name);
    }
}
