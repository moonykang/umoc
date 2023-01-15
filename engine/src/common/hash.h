#pragma once

#include "common/util.h"
#include "xxhash.h"

#define ARCH_64 1

namespace util
{
// Computes a hash of "key". Any data passed to this function must be multiples of
// 4 bytes, since the PMurHash32 method can only operate increments of 4-byte words.
inline size_t computeGenericHash(const void* key, size_t keySize)
{
    constexpr unsigned int kSeed = 0xABCDEF98;

    // We can't support "odd" alignments.  computeGenericHash requires aligned types
    ASSERT(keySize % 4 == 0);
#if defined(ARCH_64)
    return XXH64(key, keySize, kSeed);
#else
    return XXH32(key, keySize, kSeed);
#endif // defined(ARCH_64)
}

template <typename T> size_t computeGenericHash(const T& key)
{
    static_assert(sizeof(key) % 4 == 0, "computeGenericHash requires aligned types");
    return computeGenericHash(&key, sizeof(key));
}
} // namespace util