#ifdef DEBUG
#include <stdio.h>
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif
