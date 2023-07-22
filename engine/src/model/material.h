#pragma once
#include "common/util.h"
#include "defines.h"
#include "rhi/defines.h"

namespace platform
{
class Context;
}

namespace rhi
{
class Texture;
class UniformBuffer;
class DescriptorSet;
class ShaderParameters;
class StorageBuffer;
} // namespace rhi

namespace model
{

class Material
{
  public:
    Material();

    ~Material();

    virtual Result init(platform::Context* context);

    virtual Result update(platform::Context* context);

    void terminate(platform::Context* context);

    void updateTexture(MaterialFlag materialFlag, rhi::Texture* texture, rhi::ShaderStageFlags shaderStageFlags);

    void updateStorageBuffer(rhi::StorageBuffer* storageBuffer, rhi::ShaderStageFlags shaderStageFlags);

    rhi::Texture* getTexture(MaterialFlag materialFlag);

    rhi::DescriptorSet* getDescriptorSet();

    void setShaderParameters(rhi::ShaderParameters* shaderParameters);

    rhi::ShaderParameters* getShaderParameters();

  protected:
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> baseColorTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> metallicRoughnessTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> normalTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> occlusionTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> emissiveTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> specularGlossinessTexture;
    std::pair<rhi::Texture*, rhi::ShaderStageFlags> diffuseTexture;

    std::vector<std::pair<rhi::Texture*, rhi::ShaderStageFlags>> externalTextures;

    std::pair<rhi::UniformBuffer*, rhi::ShaderStageFlags> uniformBuffer;
    std::vector<std::pair<rhi::StorageBuffer*, rhi::ShaderStageFlags>> externalStorageBuffers;
    rhi::DescriptorSet* descriptorSet;

    rhi::ShaderParameters* shaderParameters;
};
} // namespace model
