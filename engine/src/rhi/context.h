#pragma once

#include "defines.h"
#include "platform/context.h"
#include "resources.h"

namespace rhi
{
class Image;
class Renderpass;

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

    virtual Result beginRenderpass(RenderPassInfo& renderpassInfo) = 0;

    virtual Result endRenderpass() = 0;
};
} // namespace rhi