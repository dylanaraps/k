void _msg(const char* type, const char *fmt, ...);

#define die(...) _msg("ERRR", __VA_ARGS__),exit(1)
#define msg(...) _msg("INFO", __VA_ARGS__)
