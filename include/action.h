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
    int opt;
    struct cache cache;
    struct repo **repos;
    pkg **pkgs;
    buf *mem;
};

enum state_options {
    STATE_CACHE     = (1L << 0),
    STATE_REPO      = (1L << 1),
    STATE_PKG       = (1L << 2),
    STATE_PKG_REPO  = (1L << 3),
    STATE_PKG_CACHE = (1L << 4),
    STATE_MEM       = (1L << 5),
};
#define STATE_ALL (~0L)

/**
 * initialize state
 */
struct state *state_init(int argc, char *argv[], int opt);

/**
 * free state
 */
void state_free(struct state *s);

/**
 * kiss checksum
 */
int action_checksum(struct state *s);

/**
 * kiss download
 */
int action_download(struct state *s);

/**
 * kiss list
 */
int action_list(struct state *s);

/**
 * kiss search
 */
int action_search(struct state *s);

#endif
