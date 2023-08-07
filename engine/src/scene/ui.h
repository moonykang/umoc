#pragma once
#include "common/util.h"

namespace platform
{
class Context;
}

namespace scene
{
class Lights;
class View;
class SceneInfo;

class UI
{
  public:
    UI();

    Result startRender(platform::Context* platformContext, SceneInfo* sceneInfo);

    Result renderSceneLight(Lights* lights);

    Result renderSceneView(View* view);

    Result endRender();

  private:
    bool enabled;
};
} // namespace scene