#pragma once

#include "common/util.h"
#include <vector>

namespace platform
{
class Context;
}

namespace scene
{
class SceneInfo;
}
namespace renderer
{
class Base
{
  public:
    virtual ~Base() = default;

    virtual Result init(platform::Context* context, scene::SceneInfo* sceneInfo) = 0;

    virtual void terminate(platform::Context* context) = 0;

    virtual Result render(platform::Context* context, scene::SceneInfo* sceneInfo) = 0;
};

class BaseRenderPass
{
  public:
    Result init(platform::Context* context, scene::SceneInfo* sceneInfo);

    void terminate(platform::Context* context);

    Result render(platform::Context* context, scene::SceneInfo* sceneInfo);

  private:
    std::vector<Base*> passes;
};
} // namespace renderer