#pragma once

#include "common/util.h"

namespace scene
{
class SceneInfo;
}

namespace renderer
{
class BaseRenderPass;
}

namespace platform
{
class Context;
class Application
{
  public:
    Application();

    ~Application() = default;

    Result init(Context* context);

    void loop(Context* context);

    void terminate(Context* context);

  private:
    scene::SceneInfo* sceneInfo;
    renderer::BaseRenderPass* baseRenderpass;
};
} // namespace platform