#pragma once

#include "common/util.h"

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
    virtual Result render(platform::Context* context, scene::SceneInfo* sceneInfo) = 0;
};

class BaseRenderPass
{
  public:
    Result render(platform::Context* context, scene::SceneInfo* sceneInfo);
};
} // namespace renderer