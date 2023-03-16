#pragma once

#include "base.h"

namespace renderer
{
class ScreenPass : public Base
{
  public:
    Result init(platform::Context* context) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;
};
} // namespace renderer