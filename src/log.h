#include <stdlib.h> /* exit */

void _msg(const char* type, const char *fmt, ...);

#define die(...) _msg("\033[31;1mERRR\033[m ", __VA_ARGS__),exit(1)
#define msg(...) _msg("", __VA_ARGS__)
