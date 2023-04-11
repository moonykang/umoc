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

    virtual Result init(platform::Context* context);

    virtual Result update(platform::Context* context);

    void terminate(platform::Context* context);

    void updateTexture(MaterialFlag materialFlag, rhi::Texture* texture);

    rhi::Texture* getTexture(MaterialFlag materialFlag);

    rhi::DescriptorSet* getDescriptorSet();

  protected:
    rhi::Texture* baseColorTexture;
    rhi::Texture* metallicRoughnessTexture;
    rhi::Texture* normalTexture;
    rhi::Texture* occlusionTexture;
    rhi::Texture* emissiveTexture;
    rhi::Texture* specularGlossinessTexture;
    rhi::Texture* diffuseTexture;

    std::vector<rhi::Texture*> externalTextures;

    rhi::UniformBuffer* uniformBuffer;
    rhi::DescriptorSet* descriptorSet;
};
} // namespace model
