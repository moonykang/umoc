#pragma once

#include "common/util.h"

namespace renderer
{
class Renderer
{
  public:
    virtual Result render() = 0;
};
} // namespace renderer