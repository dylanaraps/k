#include <stdlib.h> /* exit */

void _msg(const char* t, const char *f, const int l, const char *fmt, ...);

#define die(...) _msg("ERRR", __FILE__, __LINE__, __VA_ARGS__),exit(1)
#define msg(...) _msg("", __FILE__, __LINE__, __VA_ARGS__)
