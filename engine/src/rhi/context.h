#pragma once

#include "platform/context.h"
#include "rhi/defines.h"

namespace rhi
{
class Context : public platform::Context
{
  public:
    static Context* createRHIContext(rhi::List rhi);

  public:
    virtual Result initRHI(platform::Window* window) = 0;

    virtual void terminateRHI() = 0;

  public:
};
} // namespace rhi