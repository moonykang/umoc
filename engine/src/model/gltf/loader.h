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

        Builder& setShaderParameters(rhi::ShaderParameters* shaderParameters);

        Builder& setForcedTextureExt(std::string forcedTextureExt);

        std::shared_ptr<Loader> build();

      private:
        std::string path;
        std::string fileName;
        MaterialFlags materialFlags;
        GltfLoadingFlags gltfLoadingFlags;
        model::Material* externalMaterial;
        rhi::ShaderParameters* shaderParameters;
        std::string forcedTextureExt;
    };

    Loader(std::string path, std::string fileName, GltfLoadingFlags gltfLoadingFlags, MaterialFlags materialFlags,
           model::Material* externalMaterial, rhi::ShaderParameters* shaderParameters, std::string forcedTextureExt);

    Object* load(platform::Context* context, scene::SceneInfo* sceneInfo) override;

    Result loadTextures(platform::Context* context, scene::SceneInfo* sceneInfo, Object* object);

    Result loadMaterials(platform::Context* context, scene::SceneInfo* sceneInfo, Object* object);

    Result loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, float globalscale, Object* object);

  private:
    std::string path;
    std::string fileName;
    MaterialFlags materialFlags;
    GltfLoadingFlags gltfLoadingFlags;
    model::Material* externalMaterial;
    rhi::ShaderParameters* shaderParameters;
    std::string forcedTextureExt;

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
};
} // namespace gltf
} // namespace model
