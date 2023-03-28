#pragma once

#include "model/loader.h"
#include <memory>

namespace platform
{
class Context;
}

namespace model
{
class Material;

namespace gltf
{
class Loader : public model::Loader
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder& setPath(std::string path);

        Builder& setFileName(std::string name);

        Builder& setGltfLoadingFlags(GltfLoadingFlags gltfLoadingFlags);

        Builder& setMaterialFlags(MaterialFlags materialFlags);

        Builder& addExternalMaterial(model::Material* material);

        std::shared_ptr<Loader> build();

      private:
        std::string path;
        std::string fileName;
        MaterialFlags materialFlags;
        GltfLoadingFlags gltfLoadingFlags;
        Material* externalMaterial;
    };

    Loader(std::string path, std::string fileName, GltfLoadingFlags gltfLoadingFlags, MaterialFlags materialFlags,
           model::Material* externalMaterial);

    Object* load(platform::Context* context) override;

    Result loadTextures(platform::Context* context, Object* object);

    Result loadMaterials(platform::Context* context, Object* object);

    Result loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, float globalscale, Object* object);

  private:
    std::string path;
    std::string fileName;
    MaterialFlags materialFlags;
    GltfLoadingFlags gltfLoadingFlags;
    model::Material* externalMaterial;

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
};
} // namespace gltf
} // namespace model