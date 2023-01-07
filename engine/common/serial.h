#pragma once

#include "common/util.h"
#include <cstdlib>
#include <limits>

class ResourceSerial
{
  public:
    constexpr ResourceSerial() : mValue(kDirty)
    {
    }
    explicit constexpr ResourceSerial(uintptr_t value) : mValue(value)
    {
    }
    constexpr bool operator==(ResourceSerial other) const
    {
        return mValue == other.mValue;
    }
    constexpr bool operator!=(ResourceSerial other) const
    {
        return mValue != other.mValue;
    }

    void dirty()
    {
        mValue = kDirty;
    }
    void clear()
    {
        mValue = kEmpty;
    }

    constexpr bool valid() const
    {
        return mValue != kEmpty && mValue != kDirty;
    }
    constexpr bool empty() const
    {
        return mValue == kEmpty;
    }

  private:
    constexpr static uintptr_t kDirty = std::numeric_limits<uintptr_t>::max();
    constexpr static uintptr_t kEmpty = 0;

    uintptr_t mValue;
};

class Serial final
{
  public:
    constexpr Serial() : mValue(kInvalid)
    {
    }
    constexpr Serial(const Serial& other) = default;
    Serial& operator=(const Serial& other) = default;

    static constexpr Serial Infinite()
    {
        return Serial(std::numeric_limits<uint64_t>::max());
    }

    constexpr bool operator==(const Serial& other) const
    {
        return mValue != kInvalid && mValue == other.mValue;
    }
    constexpr bool operator==(uint32_t value) const
    {
        return mValue != kInvalid && mValue == static_cast<uint64_t>(value);
    }
    constexpr bool operator!=(const Serial& other) const
    {
        return mValue == kInvalid || mValue != other.mValue;
    }
    constexpr bool operator>(const Serial& other) const
    {
        return mValue > other.mValue;
    }
    constexpr bool operator>=(const Serial& other) const
    {
        return mValue >= other.mValue;
    }
    constexpr bool operator<(const Serial& other) const
    {
        return mValue < other.mValue;
    }
    constexpr bool operator<=(const Serial& other) const
    {
        return mValue <= other.mValue;
    }

    constexpr bool operator<(uint32_t value) const
    {
        return mValue < static_cast<uint64_t>(value);
    }

    // Useful for serialization.
    constexpr uint64_t getValue() const
    {
        return mValue;
    }
    constexpr bool valid() const
    {
        return mValue != kInvalid;
    }

  private:
    template <typename T> friend class SerialFactoryBase;
    friend class RangedSerialFactory;
    friend class AtomicQueueSerial;
    constexpr explicit Serial(uint64_t value) : mValue(value)
    {
    }
    uint64_t mValue;
    static constexpr uint64_t kInvalid = 0;
};
// Defines class to track the queue serial that can be load/store from multiple threads atomically.
class AtomicQueueSerial final
{
  public:
    constexpr AtomicQueueSerial() : mValue(kInvalid)
    {
        ASSERT(mValue.is_lock_free());
    }
    AtomicQueueSerial& operator=(const Serial& other)
    {
        mValue.store(other.mValue, std::memory_order_release);
        return *this;
    }
    Serial getSerial() const
    {
        return Serial(mValue.load(std::memory_order_consume));
    }

  private:
    std::atomic<uint64_t> mValue;
    static constexpr uint64_t kInvalid = 0;
};

// Used as default/initial serial
static constexpr Serial kZeroSerial = Serial();

// The factory to generate a serial number within the range [mSerial, mSerial+mCount}
class RangedSerialFactory final : NonCopyable
{
  public:
    RangedSerialFactory() : mSerial(0), mCount(0)
    {
    }

    void reset()
    {
        mCount = 0;
    }
    bool empty() const
    {
        return mCount == 0;
    }
    bool generate(Serial* serialOut)
    {
        if (mCount > 0)
        {
            uint64_t current = mSerial++;
            ASSERT(mSerial > current); // Integer overflow
            *serialOut = Serial(current);
            mCount--;
            return true;
        }
        return false;
    }

  private:
    template <typename T> friend class SerialFactoryBase;
    void initialize(uint64_t initialSerial, size_t count)
    {
        mSerial = initialSerial;
        mCount = count;
    }
    uint64_t mSerial;
    size_t mCount;
};

template <typename SerialBaseType> class SerialFactoryBase final : NonCopyable
{
  public:
    SerialFactoryBase() : mSerial(1)
    {
    }

    Serial generate()
    {
        uint64_t current = mSerial++;
        ASSERT(mSerial > current); // Integer overflow
        return Serial(current);
    }

    void reserve(RangedSerialFactory* rangeFactory, size_t count)
    {
        uint64_t current = mSerial;
        mSerial += count;
        ASSERT(mSerial > current); // Integer overflow
        rangeFactory->initialize(current, count);
    }

  private:
    SerialBaseType mSerial;
};

using SerialFactory = SerialFactoryBase<uint64_t>;
using AtomicSerialFactory = SerialFactoryBase<std::atomic<uint64_t>>;

// For backend that supports multiple queue serials, QueueSerial includes a Serial and an index.
using SerialIndex = uint32_t;
static constexpr SerialIndex kInvalidQueueSerialIndex = SerialIndex(-1);