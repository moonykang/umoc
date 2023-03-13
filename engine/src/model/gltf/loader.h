#pragma once

#include "model/loader.h"

namespace platform
{
class Context;
}

namespace model
{
namespace gltf
{
class Loader : public model::Loader
{
  public:
    class Builder
    {
      public:
        Builder& setPath(std::string path);

        Builder& setFileName(std::string name);

        Builder& setGltfLoadingFlags(GltfLoadingFlags gltfLoadingFlags);

        Builder& setMaterialFlags(MaterialFlags materialFlags);

        Loader* build();

      private:
        std::string path;
        std::string fileName;
        MaterialFlags materialFlags;
        GltfLoadingFlags gltfLoadingFlags;
    };

    Loader(std::string path, std::string fileName, GltfLoadingFlags gltfLoadingFlags, MaterialFlags materialFlags);

    Object* load(platform::Context* context) override;

    Result loadTextures(platform::Context* context, Object* object);

    Result loadMaterials(platform::Context* context, Object* object);

    Result loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, float globalscale, Object* object);

  private:
    std::string path;
    std::string fileName;
    MaterialFlags materialFlags;
    GltfLoadingFlags gltfLoadingFlags;

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
};
} // namespace gltf
} // namespace model