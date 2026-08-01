#include <cassert>

#define panic(message)             \
    do {                           \
        assert(false && #message); \
        throw #message;            \
    } while (0)