#pragma once

#include "model/defines.h"
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

  private:
    std::map<model::TextureID, rhi::Texture*> textureMap;
};
} // namespace scene