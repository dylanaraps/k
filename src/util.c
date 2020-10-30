#include <errno.h>
#include <glob.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "util.h"

int mkdir_p(const char* d) {
    for (char* p = strchr(d + 1, '/'); p; p = strchr(p + 1, '/')) {
        *p = 0;
        int ret = mkdir(d, 0755);
        *p = '/';

        if (ret == -1 && errno != EEXIST) {
            err("failed to create directory '%s'", d);
            return -1;
        }
    }

    return 0;
}

int run_cmd(const char *cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        err("failed to fork");
        return -1;

    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, 0);

    } else {
        int status;

        waitpid(pid, &status, 0);

        if (WEXITSTATUS(status)) {
            err("command '%s' exited non-zero", cmd);
            return -1;
        }
    }

    return 0;    
}

int is_dir(const char *path) {
   struct stat statbuf;

   if (stat(path, &statbuf) != 0) {
       err_no("failed to stat path '%s'", path);
       return 0;
   }

   return S_ISDIR(statbuf.st_mode);
}

int file_print_line(FILE *f) {
    for (char c; (c = fgetc(f)) != '\n' && c != EOF; ) {
        putchar(c);
    }
    putchar('\n');

    return ferror(f);
}
