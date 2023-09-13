#pragma once
#include "common/util.h"

namespace scene
{
class View;
}
namespace ui
{
class Component
{
  public:
    virtual Result updateUI(scene::View* view) = 0;
};
} // namespace ui