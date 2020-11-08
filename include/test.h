#ifndef KISS_TEST_H
#define KISS_TEST_H

#define test(x) test_internal(#x, !!(x), __LINE__)

int test_begin(const char *file);
void test_internal(const char *expr, int result, int line);
int test_finish(void);

#endif
