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
    Result init(platform::Context* context);

    Result render(platform::Context* context);
};
} // namespace renderer