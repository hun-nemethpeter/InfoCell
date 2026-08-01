#include <cassert>

#define panic(message)             \
    do {                           \
        assert(false && #message); \
    } while (0)