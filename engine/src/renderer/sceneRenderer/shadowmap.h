#pragma once

#include "../base.h"
#include "rhi/defines.h"

namespace model
{
class Object;
class Instance;
} // namespace model

namespace rhi
{
class ShaderParameters;
}

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