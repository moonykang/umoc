#pragma once

#include "common/util.h"

namespace scene
{
class SceneInfo;
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
};
} // namespace platform