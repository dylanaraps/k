/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ACTION_H
#define KISS_ACTION_H

#include "buf.h"

/**
 * kiss list
 */
int action_list(buf **buf, int argc, char *argv[]);

/**
 * kiss search
 */
int action_search(buf **buf, int argc, char *argv[]);

#endif
