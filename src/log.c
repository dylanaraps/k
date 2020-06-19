#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "log.h"

static const char *levels[] = {
    "ERRR", "WARN", "INFO"
};

void log_log(int level, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    fprintf(stderr, "[\033[3%d;1m%s\033[m] ", level + 1, levels[level]);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}
