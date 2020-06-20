#include <stdio.h>  /* vfprintf, fprintf */
#include <stdarg.h> /* va_list, va_start, va_end */

#include "log.h"

void _msg(const char* type, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    printf("| [%s] ", type);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}
