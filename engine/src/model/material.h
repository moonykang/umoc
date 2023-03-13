#pragma once
#include "common/util.h"
#include "defines.h"

namespace platform
{
class Context;
}

namespace rhi
{
class Texture;
class UniformBuffer;
class DescriptorSet;
} // namespace rhi

namespace model
{

class Material
{
  public:
    Material();

    ~Material() = default;

    Result init(platform::Context* context);

    Result update(platform::Context* context);

    void terminate(platform::Context* context);

    void updateTexture(MaterialFlag materialFlag, rhi::Texture* texture);

    rhi::Texture* getTexture(MaterialFlag materialFlag);

    rhi::DescriptorSet* getDescriptorSet();

    void setAlphaCutoff(float v);

    void setMetallicFactor(float v);

    void setRoughnessFactor(float v);

    void setBaseColorFactor(glm::vec4 colorFactor);

    void setAlphaMode(AlphaMode alphaMode);

  private:
    struct MaterialUniformBlock
    {
        glm::vec4 materialFactors; // x: alphaCutoff, y: metallicFactor, z: roughnessFactor, w: reserved
        glm::vec4 baseColorFactor;
        AlphaMode alphaMode; // align?

        MaterialUniformBlock();
    } ubo;

    rhi::Texture* baseColorTexture;
    rhi::Texture* metallicRoughnessTexture;
    rhi::Texture* normalTexture;
    rhi::Texture* occlusionTexture;
    rhi::Texture* emissiveTexture;
    rhi::Texture* specularGlossinessTexture;
    rhi::Texture* diffuseTexture;

    rhi::UniformBuffer* uniformBuffer;
    rhi::DescriptorSet* descriptorSet;
};
} // namespace model
