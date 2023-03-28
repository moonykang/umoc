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

#define REGISTER_TEXTURE(texture, getFunc)                                                                             \
  public:                                                                                                              \
    inline rhi::Texture* getFunc()                                                                                     \
    {                                                                                                                  \
        return texture;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    rhi::Texture* texture;

    REGISTER_TEXTURE(sceneColor, getSceneColor);
    REGISTER_TEXTURE(sceneDepth, getSceneDepth);
    REGISTER_TEXTURE(brdfLutTexture, getBrdfLutTexture);
    REGISTER_TEXTURE(environmentCube, getEnvironmentCube);
    REGISTER_TEXTURE(irradianceCube, getIrradianceCube);
    REGISTER_TEXTURE(preFilterCube, getPreFilterCube);
};
} // namespace scene