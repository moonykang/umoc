#pragma once

#include "base.h"

namespace model
{
class Object;
class Instance;
} // namespace model

namespace renderer
{
class SSAOPass : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    model::Object* object;
    model::Instance* instance;

    model::Object* blurObject;
    model::Instance* blurInstance;

    model::Object* lightingObject;
    model::Instance* lightingInstance;
};
} // namespace renderer