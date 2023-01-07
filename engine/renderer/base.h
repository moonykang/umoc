#pragma once

#include "common/util.h"

namespace renderer
{
class Base
{
  public:
    virtual Result render() = 0;
};
} // namespace renderer