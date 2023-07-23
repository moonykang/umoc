#pragma once

#include "common/util.h"
#include <chrono>

namespace platform
{
class Timer
{
  public:
    Timer() : current(0), frameTimer(0), last(std::chrono::high_resolution_clock::now())
    {
    }

    uint64_t getCurrentTime()
    {
        return current;
    }

    float getCurrentFrameTimer()
    {
        return frameTimer;
    }

    void tick()
    {
        auto now = std::chrono::high_resolution_clock::now();
        current++;

        auto tDiff = std::chrono::duration<double, std::milli>(now - last).count();
        frameTimer = (float)tDiff / 1000.0f;

        last = now;
    }

  private:
    uint64_t current;
    float frameTimer;
    std::chrono::steady_clock::time_point last;
};
} // namespace platform