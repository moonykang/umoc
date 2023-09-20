
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
}

namespace renderer
{
class GridPass : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    model::Object* object;
    model::Instance* instance;
};
} // namespace renderer