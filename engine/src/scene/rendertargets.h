#pragma once

#include "common/util.h"

namespace rhi
{
class Texture;
}

namespace platform
{
class Context;
}

namespace scene
{
class RenderTargets
{
  public:
    RenderTargets();

    virtual ~RenderTargets() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    rhi::Texture* getSceneColor();

    rhi::Texture* getSceneDepth();

    rhi::Texture* getBrdfLutTexture();

  private:
    rhi::Texture* sceneColor;
    rhi::Texture* sceneDepth;
    rhi::Texture* brdfLutTexture;
};
} // namespace scene