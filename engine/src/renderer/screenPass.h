#pragma once

#include "base.h"

namespace renderer
{
class ScreenPass : public Base
{
  public:
    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;
};
} // namespace renderer