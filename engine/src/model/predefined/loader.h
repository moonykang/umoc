#pragma once

#include "common/util.h"
#include "model/loader.h"
#include <map>
#include <memory>

namespace rhi
{
class SubAllocatedBuffer;
}

namespace model
{
class Material;
namespace predefined
{
class Loader : public model::Loader
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder& setPredefineModelType(PredefinedModel predefinedType);

        Builder& setMaterial(Material* material);

        Builder& setShaderParameters(rhi::ShaderParameters* shaderParameters);

        Builder& setUvScale(float uvScale);

        Builder& setExternalVertexBuffer(std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer);

        std::shared_ptr<Loader> build();

      private:
        PredefinedModel predefinedType;
        Material* material;
        rhi::ShaderParameters* shaderParameters;
        std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer;
        float uvScale;
    };

  public:
    Loader(PredefinedModel predefinedType, Material* material, rhi::ShaderParameters* shaderParameters, float uvScale,
           std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer);

    Object* load(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    PredefinedModel predefinedType;
    Material* material;
    rhi::ShaderParameters* shaderParameters;
    std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer;
    float uvScale;
};
} // namespace predefined
} // namespace model