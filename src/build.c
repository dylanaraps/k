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
    int child;

    SAVE_CWD;

    mkchdir(pkg->dest_dir);
    mkchdir(pkg->build_dir);

    if (access(pkg->build, X_OK) == -1)
        log_error("Build file not executable");
    
    switch (child = fork()) {
    case -1:
        log_error("fork() failed");
        break;

    case 0:
        execvp(pkg->build, (char*[]){ 
            pkg->build, 
            pkg->dest_dir, 
            pkg->ver, 
            NULL 
        });
        break;

    default:
        waitpid(child, NULL, 0);
    }

    LOAD_CWD;
}
