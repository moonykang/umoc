#pragma once

#include "common/memorybuffer.h"
#include "model/defines.h"
#include "rhi/defines.h"
#include <map>
#include <string>
#include <utility>

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
class Textures
{
  public:
    void terminate(platform::Context* context);

    rhi::Texture* get(model::TextureID id);

    std::pair<model::TextureID, rhi::Texture*> get(platform::Context* platformContext, std::string name,
                                                   std::string fullpath);

    std::pair<model::TextureID, rhi::Texture*> get(platform::Context* platformContext, std::string name,
                                                   rhi::Format format, rhi::Extent3D extent, size_t size, void* data);

  private:
    std::map<model::TextureID, rhi::Texture*> textureMap;
};
} // namespace scene