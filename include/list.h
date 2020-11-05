/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_LIST_H
#define KISS_LIST_H

typedef struct {
	void **arr;
	size_t cap;
	size_t len;
} list;

/**
 * Initialize list and allocate memory for s elements. If s is 0, a default
 * value of 16 is used. Returns -ENOMEM when memory allocation fails or if
 * calculation of memory size overflows.
 */
int list_init(list *l, size_t s);

/**
 * Push an element to the list growing the list (by a factor of 1.5) if needed.
 * Returns 0 for success and -ENOMEM for failure.
 */
int list_push_b(list *l, void *d);

/**
 * Drop an element from the end of the list.
 */
void list_drop_b(list *l);

/**
 * Sort a list using qsort().
 */
void list_sort(list *l, int (*cb)(const void *, const void *));

/**
 * Free memory associated with the list. Checks for NULL before calling
 * free(). List elements themselves must be freed by the caller.
 */
void list_free(list *l);

#endif
