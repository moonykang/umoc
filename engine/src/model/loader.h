#pragma once

#include "common/util.h"
#include "defines.h"
#include "rhi/defines.h"
#include <string>
#include <vector>

namespace platform
{
class Context;
}

namespace scene
{
class SceneInfo;
}

namespace model
{
class Object;
class Node;

class Loader
{
  public:
    virtual Object* load(platform::Context* context, scene::SceneInfo* sceneInfo) = 0;

    Result loadMaterial(platform::Context* context, Object* object);

  protected:
    std::vector<rhi::Vertex> vertices;
    std::vector<uint32_t> indices;
};
} // namespace model