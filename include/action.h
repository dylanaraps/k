/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ACTION_H
#define KISS_ACTION_H

#include "buf.h"
#include "cache.h"
#include "pkg.h"
#include "repo.h"

struct state {
   struct cache cache;
   struct repo **repos;
   pkg **pkgs;
   buf *mem;
};

/**
 * initialize state
 */
struct state *state_init(int argc, char *argv[]);

/**
 * free state
 */
void state_free(struct state *s);

/**
 * kiss download
 */
int action_download(struct state *s);

/**
 * kiss list
 */
int action_list(int argc, char *argv[]);

/**
 * kiss search
 */
int action_search(int argc, char *argv[]);

#endif
