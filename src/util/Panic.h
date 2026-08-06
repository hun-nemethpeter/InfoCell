#include <cassert>
#include <cstdlib>

#define panic(message)             \
    do {                           \
        assert(false && #message); \
        ::abort();                 \
    } while (0)