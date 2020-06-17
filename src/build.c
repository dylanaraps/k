#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "pkg.h"
#include "log.h"
#include "version.h"
#include "util.h"
#include "pkg.h"
#include "build.h"

void pkg_build(package *pkg) {
    char build_script[PATH_MAX + 1];
    char pkg_dir[PATH_MAX + 1];
    int child;

    pkg->version = pkg_version(pkg->path[0]);

    xchdir(PKG_DIR);
    mkdir(pkg->name, 0777);
    xchdir(pkg->name);
    xchdir(MAK_DIR);
    xchdir(pkg->name);

    sprintf(build_script, "%s/build",  pkg->path[0]);
    sprintf(pkg_dir, "%s/%s",  PKG_DIR, pkg->name);

    if (access(build_script, X_OK) == -1)
        log_error("Build file not executable");

    child = fork();

    switch (child) {
    case -1:
        log_error("fork() failed");
        break;

    case 0:
        execvp(build_script, (char*[]){ 
            build_script, 
            pkg_dir, 
            pkg->version.version, 
            NULL 
        });
        break;
    }

    waitpid(child, NULL, 0);
}
