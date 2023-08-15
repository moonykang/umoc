#pragma once
#include "common/util.h"

namespace ui
{
class Component
{
  public:
    virtual Result updateUI() = 0;
};
} // namespace ui