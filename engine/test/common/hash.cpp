#include "common/hash.h"
#include "common/util.h"
#include <array>
#include <cstdio>
#include <gtest/gtest.h>
#include <string>

class ChildStruct
{
  public:
    int a;
    int b;
};

class ChildStruct2
{
  public:
    float a;
    float b;
};

class HashTestStruct
{
  public:
    ChildStruct c1;
    ChildStruct2 c2;
    std::array<uint32_t, 5> arr;
};

class StringStruct
{
  public:
    std::string s;
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

    LOGD("%zu %zu %zu", sizeof(ChildStruct), sizeof(ChildStruct2), 5 * sizeof(uint32_t));

    size_t hash_h1 = util::computeGenericHash(&h1, sizeof(HashTestStruct));
    size_t hash_h2 = util::computeGenericHash(&h2, sizeof(HashTestStruct));

    EXPECT_NE(hash_h1, hash_h2);
}

TEST(StringStructTest, BasicAssertions)
{
    StringStruct s1;
    StringStruct s2;
    StringStruct s3;

    s1.s = "abcdedsdabcdsdsdsdsdf";
    s2.s = "abcdedsdabcdsdsdsdsdg";
    s3.s = "abcdedsdabcdsdsdsdsdf";

    size_t size = sizeof(StringStruct);
    std::string str = "abcdedsdabcdsdsdsdsd";
    LOGD("size of StringStruct: %zu string %zu str %zu", size, sizeof(std::string), sizeof(str));

    size_t hash_h1 = util::computeGenericHash(&s1, sizeof(StringStruct));
    size_t hash_h2 = util::computeGenericHash(&s2, sizeof(StringStruct));
    size_t hash_h3 = util::computeGenericHash(&s3, sizeof(StringStruct));

    LOGD("%zu %zu %zu", hash_h1, hash_h2, hash_h3);

    EXPECT_NE(hash_h1, hash_h2);
    EXPECT_EQ(hash_h1, hash_h3);
}