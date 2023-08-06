#pragma once

#include "../base.h"

namespace rhi
{
class ShaderParameters;
} // namespace rhi

namespace renderer
{
class ShadowMap : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    rhi::ShaderParameters* shaderParameters;
};
} // namespace renderer