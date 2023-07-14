#pragma once

#include "../base.h"

namespace model
{
class Object;
class Instance;
} // namespace model

namespace renderer
{
class BloomPass : public Base
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    void terminate(platform::Context* context) override;

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    model::Object* bloomSetupObject;
    model::Instance* bloomSetupInstance;

    model::Object* bloomHorizontalObject;
    model::Instance* bloomHorizontalInstance;

    model::Object* bloomVerticalObject;
    model::Instance* bloomVerticalInstance;

    model::Object* bloomCompositeObject;
    model::Instance* bloomCompositeInstance;
};
} // namespace renderer