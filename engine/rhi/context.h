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
    virtual void initRHI() = 0;
};
} // namespace rhi