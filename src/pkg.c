#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "util.h"
#include "vec.h"
#include "str.h"
#include "pkg.h"

package pkg_init(const char *name) {
    return (package) {
        .name = strdup(name),
    };
}

void pkg_free(package *pkgs) {
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        free(pkgs[i].name);
    }
    vec_free(pkgs);
}

package *pkg_init_db(void) {
    package *pkgs = NULL;
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

    return pkgs;
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
