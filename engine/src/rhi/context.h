#pragma once

#include "platform/context.h"
#include "rhi/defines.h"

namespace rhi
{
class Image;

class Context : public platform::Context
{
  public:
    static Context* createRHIContext(rhi::List rhi);

  public:
    virtual Result initRHI(platform::Window* window) = 0;

    virtual void terminateRHI() = 0;

    virtual Result flush() = 0;

    virtual Result present() = 0;

    virtual Image* getCurrentSurfaceImage() = 0;
};
} // namespace rhi