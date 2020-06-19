#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#include "pkg.h"

enum { M_ERRR, M_WARN, M_INFO };

void _msg(const char* type, const char *fmt, ...);

#define die(...) log_log(0, __VA_ARGS__),exit(1)
#define war(...) log_log(1, __VA_ARGS__)
#define msg(...) log_log(2, __VA_ARGS__)

#endif
