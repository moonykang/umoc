#pragma once

#include "asset.h"
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

    virtual Result initRHI() = 0;

    void terminate();

    virtual void terminateRHI() = 0;

    rhi::Context* getRHI();

  private:
    Asset* asset;

  protected:
    Window* window;
};
} // namespace platform