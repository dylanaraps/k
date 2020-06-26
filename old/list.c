#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>

#include "log.h"
#include "pkg.h"
#include "vec.h"
#include "util.h"
#include "list.h"

int pkg_list(const char *pkg_name) {
    return exists_at(DB_DIR, pkg_name, O_DIRECTORY);
}

void pkg_list_all(package *pkg) {
    package *tmp = pkg;
    struct dirent  **list;
    int err;

    if (!pkg) {
        err = scandir(DB_DIR, &list, NULL, alphasort);

        if (err == -1) {
            die("Package DB not accessible");
        }

        for (int i = 0; i < err; i++) {
            if (list[i]->d_name[0] == '.' && !list[i]->d_name[2]) {
                continue;
            }

            vec_push_back(tmp, pkg_init(list[i]->d_name));
            free(list[i]);
        }

        free(list);
    }

    if (tmp) {
        /* yucky global for atexit :( */
        PKG = tmp;

        for (size_t i = 0; i < vec_size(tmp); ++i) {
            if (pkg_list(tmp->name) != 0) {
                die("[%s] Package not installed", tmp[i].name);
            }

            printf("%s %s %s\n", tmp[i].name, tmp[i].ver, tmp[i].rel);
        }
    }
}
