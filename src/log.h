#define _POSIX_C_SOURCE 200809L
/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#include "pkg.h"

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_INFO, LOG_WARN, LOG_ERROR };

#define msg(...) log_log(LOG_INFO,  PKG, __VA_ARGS__)
#define war(...) log_log(LOG_WARN,  PKG, __VA_ARGS__)
#define die(...) log_log(LOG_ERROR, PKG, __VA_ARGS__),exit(1)

void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_fp(FILE *fp);
void log_set_level(int level);
void log_set_quiet(int enable);

void log_log(int level, const char *file, const char *fmt, ...);

#endif
