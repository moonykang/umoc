#include "common/hash.h"
#include "common/util.h"
#include <array>
#include <cstdio>
#include <gtest/gtest.h>

struct HashTestStruct
{
    std::array<uint32_t, 5> arr;
};

// Demonstrate some basic assertions.
TEST(HashStructTest, BasicAssertions)
{
    HashTestStruct h1;
    HashTestStruct h2;

    {
        h1.arr = {1, 2, 3, 4, 5};
    }

    {
        h2.arr = {2, 3, 4, 5, 6};
    }

    size_t size = sizeof(HashTestStruct);
    LOGD("size of HashTestStruct: %zu", size);

    size_t hash_h1 = util::computeGenericHash(&h1, sizeof(HashTestStruct));
    size_t hash_h2 = util::computeGenericHash(&h2, sizeof(HashTestStruct));

    EXPECT_NE(hash_h1, hash_h2);
}