#include <stdio.h>  /* vfprintf, fprintf */
#include <stdarg.h> /* va_list, va_start, va_end */

#include "log.h"

void _msg(const char* t, const char *f, const int l, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%d) ", t, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}
