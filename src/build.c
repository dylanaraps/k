#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "pkg.h"
#include "version.h"
#include "build.h"

void pkg_build(package *pkg) {
    char build_script[PATH_MAX + 1];
    char pkg_dir[PATH_MAX + 1];

    pkg->version = pkg_version(pkg->path[0]);

    if (chdir(PKG_DIR) != 0) {
        printf("error: Package directory not accessible\n");
        exit(1);
    }

    mkdir(pkg->name, 0777);

    if (chdir(pkg->name) != 0) {
        printf("error: Package directory not accessible\n");
        exit(1);
    }

    if (chdir(MAK_DIR) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }

    if (chdir(pkg->name) != 0) {
        printf("error: Cache directory not accessible\n");
        exit(1);
    }

    strcpy(build_script, pkg->path[0]);
    strcat(build_script, "/build\0");
    strcpy(pkg_dir, PKG_DIR);
    strcat(pkg_dir, "/");
    strcat(pkg_dir, pkg->name);

    if (access(build_script, X_OK) == -1) {
        printf("error: Build file not executable\n");
        exit(1);
    }

    execvp(build_script, (char*[]){ build_script, 
                                    pkg_dir, 
                                    pkg->version.version, 
                                    NULL 
                                  });
}
