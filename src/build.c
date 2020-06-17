#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "pkg.h"
#include "log.h"
#include "version.h"
#include "build.h"

void pkg_build(package *pkg) {
    char build_script[PATH_MAX + 1];
    char pkg_dir[PATH_MAX + 1];

    pkg->version = pkg_version(pkg->path[0]);

    if (chdir(PKG_DIR) != 0) {
        log_error("Package directory not accessible (%s)", PKG_DIR);
    }

    mkdir(pkg->name, 0777);

    if (chdir(pkg->name) != 0) {
        log_error("Directory not accessible (%s/%s)", PKG_DIR, pkg->name);
    }

    if (chdir(MAK_DIR) != 0) {
        log_error("Cache not accessible (%s/%s)", MAK_DIR);
    }

    if (chdir(pkg->name) != 0) {
        log_error("Cache not accessible (%s/%s)", MAK_DIR);
    }

    strcpy(build_script, pkg->path[0]);
    strcat(build_script, "/build\0");
    strcpy(pkg_dir, PKG_DIR);
    strcat(pkg_dir, "/");
    strcat(pkg_dir, pkg->name);

    if (access(build_script, X_OK) == -1) {
        log_error("Build file not executable");
    }

    execvp(build_script, (char*[]){ build_script, 
                                    pkg_dir, 
                                    pkg->version.version, 
                                    NULL 
                                  });
}
