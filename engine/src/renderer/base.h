#pragma once

#include "common/util.h"

namespace platform
{
class Context;
}

namespace renderer
{
class Base
{
  public:
    virtual Result render(platform::Context* context) = 0;
};

class BaseRenderPass
{
  public:
    Result render(platform::Context* context);
};
} // namespace renderer