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
    virtual Result init(platform::Context* context);

    virtual Result render(platform::Context* context);
};
} // namespace renderer