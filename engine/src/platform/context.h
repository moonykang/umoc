#pragma once

#include "asset.h"
#include "timer.h"
#include "window.h"

namespace rhi
{
class Context;
}

namespace platform
{
class Context
{
  public:
    Context();

    virtual ~Context() = default;

    Result init();

    virtual Result initRHI(Window* window) = 0;

    void terminate();

    Result loop();

    virtual void terminateRHI() = 0;

    rhi::Context* getRHI();

    Asset* getAssetManager();

    Window* getWindow();

    Timer& getTimer();

  protected:
    Window* window;
    Asset* asset;
    Timer timer;
};
} // namespace platform