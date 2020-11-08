#include <stdio.h>
#include <stdlib.h>

#include "test.h"

static int test_count;
static int test_error_count;

int test_begin(const char *file) {
    printf("Running tests for %s:\n", file);

    test_count = 0;
    test_error_count = 0;

    return 0;
}

void test_internal(const char *expr, int result, int line) {
    printf("[line %03d] %s %s\n", line, result ? "PASS" : "FAIL", expr);

    test_error_count += result ? 0 : 1;
    test_count++;
}

int test_finish(void) {
    printf("Ran %d tests, %d succeeded, %d failed\n",
        test_count, test_count - test_error_count, test_error_count);

    return test_error_count ? 1 : 0;
}

