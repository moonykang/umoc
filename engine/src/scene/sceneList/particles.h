#pragma once

#include "../scene.h"
#include <vector>

namespace model
{
class VertexInput;
class Instance;
} // namespace model

namespace rhi
{
class UniformBuffer;
class DescriptorSet;
class Texture;
} // namespace rhi

namespace scene
{
class ParticleScene : public SceneInfo
{
  public:
    Result load(platform::Context* context) override;
};
} // namespace scene