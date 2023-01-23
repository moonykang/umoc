#pragma once

#if PLATFORM_MAC
#include <cstdio>
#define LOGD(msg, ...) printf("[%s:%u] " msg "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGE(msg, ...) fprintf(stderr, "[%s:%u] " msg "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGD(msg, ...)
#define LOGE(msg, ...)
#endif

#include <cassert>
void print_trace();
#define ASSERT(expr)                                                                                                   \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        print_trace();                                                                                                 \
        assert(true);                                                                                                  \
    }

#define UNREACHABLE() ASSERT(true)