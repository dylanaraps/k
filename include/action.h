/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ACTION_H
#define KISS_ACTION_H

#include "str.h"

/**
 * kiss list
 */
int action_list(str **buf, int argc, char *argv[]);

/**
 * kiss build
 */
int action_search(int argc, char *argv[]);

#endif
