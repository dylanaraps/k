#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>   /* malloc, size_t */
#include <stdio.h>    /* printf */
#include <limits.h>   /* PATH_MAX */
#include <string.h>   /* strncpy */
#include <sys/stat.h> /* mkdir */
#include <errno.h>    /* errno, EEXIST, S_IRWXU */

#include "log.h"
#include "strl.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    if (n == 0) {
        die("Empty memory allocation");    
    }

    p = malloc(n);

    if (!p) {
        die("Failed to allocate memory");
    }

    return p;
}

int cntchr(const char *str, int chr) {
    const char *tmp = str;
    int i = 0;

    for (; tmp[i]; tmp[i] == chr ? i++ : *tmp++);   

    return i;
}

void mkdir_p(const char *dir) {
    char tmp[PATH_MAX];   
    int err;
    char *p = 0;

    if (!dir) {
        die("mkdir input empty");
    }

    err = strlcpy(tmp, dir, PATH_MAX);
    
    if (err > PATH_MAX) {
        die("strlcpy truncated PATH");
    }

    for (p = tmp + 1; *p; p++) {
       if (*p == '/') {
           *p = 0;

           err = mkdir(tmp, S_IRWXU);

           if (err == -1 && errno != EEXIST) {
               die("Failed to create directory %s", tmp);    
           }

           *p = '/';
       }
    }

    err = mkdir(tmp, S_IRWXU);

    if (err == -1 && errno != EEXIST) {
        die("Failed to create directory %s", tmp);    
    }
}
