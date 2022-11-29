#pragma once

#if PLATFORM_MAC
#include <cstdio>
#define LOGD(msg, ...) printf("[%s:%u] " msg "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGD(msg, ...)
#endif

#define ASSERT(expr)