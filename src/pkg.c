#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include "str.h"
#include "log.h"
#include "repo.h"
#include "pkg.h"

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

void pkg_list_print(str **s, char *name) {
    pkg_version(s, name, get_db_dir());

    if ((*s)->err == STR_OK) {
        printf("%s %s\n", name, (*s)->buf);

    } else {
        die("package '%s' not installed", name);
    }
}

void pkg_list_all(str **s) {
    struct dirent **list;

    int len = scandir(get_db_dir(), &list, 0, alphasort);

    if (len > 0) {
        free(list[0]);
        free(list[1]);

        for (int i = 2; i < len; i++) {
            pkg_list_print(s, list[i]->d_name);
            free(list[i]);
        }

        free(list);

    } else {
        die("database not accessible");
    }
}

