#pragma once

#include "base.h"

namespace model
{
class VertexInput;
} // namespace model

namespace rhi
{
class Texture;
class DescriptorSet;
} // namespace rhi

namespace renderer
{
class UIPass : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    Result updateUI();
    Result updateBuffers(platform::Context* context);

  private:
    model::VertexInput* vertexInput;
    rhi::Texture* fontTexture;
    rhi::DescriptorSet* uiDescriptorSet;
};
} // namespace renderer