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

    void init();

    virtual void initRHI() = 0;

    void terminate();

    rhi::Context* getRHI();

  private:
    Asset* asset;

  protected:
    Window* window;
};
} // namespace platform