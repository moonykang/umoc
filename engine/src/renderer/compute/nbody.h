#pragma once

#include "../base.h"

namespace model
{
class Object;
class Instance;
} // namespace model

namespace rhi
{
class Texture;
class UniformBuffer;
} // namespace rhi

namespace renderer
{
class NBodyPass : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    model::Object* calculateObject;
    model::Instance* calculateInstance;

    model::Object* integrateObject;
    model::Instance* integrateInstance;

    model::Object* graphicsObject;
    model::Instance* graphicsInstance;

    double animTimer = 0;
    uint32_t numParticles;

    rhi::UniformBuffer* computeUniformBuffer;
};
} // namespace renderer