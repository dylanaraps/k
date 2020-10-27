#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "util.h"

const char *xgetenv(const char *var, const char *fallback) {
    char *env = getenv(var);

    return env ? env : fallback;
}

char *path_normalize(char *d) {
    if (d) {
        for (size_t i = 1, l = strlen(d);
             d[l - i] == '/';
             d[l - i] = 0, i++);
    }

    return d;
}

void mkdir_at_die(int fd, const char *d) {
    if (mkdirat(fd, d, 0755) == -1) {
        switch (errno) {
            case EEXIST:
                break;

            default:
                close(fd);
                die("failed to create directory '%s': %s", d, strerror(errno));
        }
    }
}

void mkdir_die(const char *d) {
    mkdir_at_die(AT_FDCWD, d);
}

void mkdir_at_str(const char *p, const char *d) {
    int fd = open(p, O_RDONLY);    

    if (fd == -1) {
        die("failed to open directory %s: %s", p, strerror(errno));
    }

    mkdir_at_die(fd, d);
    close(fd);
}

void mkdir_p_die(const char *d) {
    if (mkdir_p(d) == -1) {
        switch (errno) {
            case EEXIST:
                break;

            case ENOENT:
                die("directory component does not exist '%s'", d);

            default:
                die("failed to create directory '%s': %s", d, strerror(errno));
        }
    }
}

int mkdir_p(const char* d) {
    for (char* p = strchr(d + 1, '/'); p; p = strchr(p + 1, '/')) {
        *p = '\0';

        if (mkdir(d, 0755) == -1 && errno != EEXIST) {
            *p = '/';
            return -1;
        }

        *p = '/';
    }

    return 0;
}

int run_cmd(const char *cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        return -1;

    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, 0);

    } else {
        int status;

        waitpid(pid, &status, 0);

        if (WEXITSTATUS(status)) {
            return -1;
        }
    }

    return 0;    
}

int is_dir(const char *path) {
   struct stat statbuf;

   if (stat(path, &statbuf) != 0) {
       return 0;
   }

   return S_ISDIR(statbuf.st_mode);
}
