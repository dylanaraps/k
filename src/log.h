#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#include "pkg.h"

enum { M_INFO, M_WARN, M_ERRR };

void log_log(int level, const char *fmt, ...);

#define msg(...) log_log(M_INFO,  __VA_ARGS__)
#define war(...) log_log(M_WARN,  __VA_ARGS__)
#define die(...) log_log(M_ERRR, __VA_ARGS__),exit(1)

#endif
