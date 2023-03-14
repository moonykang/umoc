#include "material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/resources.h"

namespace model
{
Material::MaterialUniformBlock::MaterialUniformBlock()
    : alphaMode(AlphaMode::Opaque), materialFactors(glm::vec4(1.f)), baseColorFactor(glm::vec4(1.f))
{
}

void Material::setAlphaCutoff(float alphaCutoff)
{
    ubo.materialFactors.x = alphaCutoff;
}

void Material::setMetallicFactor(float metallicFactor)
{
    ubo.materialFactors.y = metallicFactor;
}

void Material::setRoughnessFactor(float roughnessFactor)
{
    ubo.materialFactors.z = roughnessFactor;
}

void Material::setBaseColorFactor(glm::vec4 colorFactor)
{
    ubo.baseColorFactor = colorFactor;
}

void Material::setAlphaMode(AlphaMode alphaMode)
{
    ubo.alphaMode = alphaMode;
}

Material::Material()
    : ubo(), baseColorTexture(nullptr), metallicRoughnessTexture(nullptr), normalTexture(nullptr),
      occlusionTexture(nullptr), emissiveTexture(nullptr), specularGlossinessTexture(nullptr), diffuseTexture(nullptr),
      uniformBuffer(nullptr), descriptorSet(nullptr)
{
}

Result Material::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    uniformBuffer = context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo);
    descriptorSet = context->allocateDescriptorSet();

    return Result::Continue;
}

Result Material::update(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uint32_t binding = 0;
    rhi::DescriptorInfoList descriptorInfoList;

    try(uniformBuffer->update(context, sizeof(MaterialUniformBlock), &ubo));

    rhi::DescriptorList descriptorList;

    descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic});
    descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic},
                              uniformBuffer->getDescriptor()});

    if (baseColorTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  baseColorTexture->getDescriptor()});
    }

    if (metallicRoughnessTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  metallicRoughnessTexture->getDescriptor()});
    }

    if (normalTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  normalTexture->getDescriptor()});
    }

    if (occlusionTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  occlusionTexture->getDescriptor()});
    }

    if (emissiveTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  uniformBuffer->getDescriptor()});
    }

    if (specularGlossinessTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  specularGlossinessTexture->getDescriptor()});
    }

    if (diffuseTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  diffuseTexture->getDescriptor()});
    }

    try(descriptorSet->init(context, descriptorInfoList));
    return Result::Continue;
}

void Material::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    TERMINATE(descriptorSet, context);
}

void Material::updateTexture(MaterialFlag materialFlag, rhi::Texture* texture)
{
    switch (materialFlag.get())
    {
    case MaterialFlag::BaseColorTexture:
        baseColorTexture = texture;
        break;
    case MaterialFlag::MetalicRoughnessTexture:
        metallicRoughnessTexture = texture;
        break;
    case MaterialFlag::NormalTexture:
        normalTexture = texture;
        break;
    case MaterialFlag::OcclusionTexture:
        occlusionTexture = texture;
        break;
    case MaterialFlag::EmissiveTexture:
        emissiveTexture = texture;
        break;
    case MaterialFlag::SpecularGlossinessTexture:
        specularGlossinessTexture = texture;
        break;
    case MaterialFlag::DiffuseTexture:
        diffuseTexture = texture;
        break;
    }
}

rhi::Texture* Material::getTexture(MaterialFlag materialFlag)
{
    switch (materialFlag.get())
    {
    case MaterialFlag::BaseColorTexture:
        return baseColorTexture;
    case MaterialFlag::MetalicRoughnessTexture:
        return metallicRoughnessTexture;
    case MaterialFlag::NormalTexture:
        return normalTexture;
    case MaterialFlag::OcclusionTexture:
        return occlusionTexture;
    case MaterialFlag::EmissiveTexture:
        return emissiveTexture;
    case MaterialFlag::SpecularGlossinessTexture:
        return specularGlossinessTexture;
    case MaterialFlag::DiffuseTexture:
        return diffuseTexture;
    default:
        UNREACHABLE();
        return nullptr;
    }
}

rhi::DescriptorSet* Material::getDescriptorSet()
{
    ASSERT(descriptorSet);
    return descriptorSet;
}
} // namespace model