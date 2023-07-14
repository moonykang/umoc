#pragma once

#include "common/util.h"

namespace platform
{
class Timer
{
  public:
    Timer() : current(0)
    {
    }

    uint64_t getCurrentTime()
    {
        return current;
    }

    void tick()
    {
        current++;
    }

  private:
    uint64_t current;
};
} // namespace platform