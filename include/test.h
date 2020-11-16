/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_TEST_H
#define KISS_TEST_H

/**
 * Similar to assert() only it will not abort.
 */
#define test(x) test_internal(#x, !!(x), __LINE__)

/**
 * Setup environment for testing.
 */
int test_begin(const char *file);

/**
 * Code backing for test() macro.
 */
void test_internal(const char *expr, int result, int line);

/**
 * Teardown environment for testing.
 */
int test_finish(void);

#endif
