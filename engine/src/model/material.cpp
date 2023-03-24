#include "material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/resources.h"

namespace model
{
Material::Material()
    : baseColorTexture(nullptr), metallicRoughnessTexture(nullptr), normalTexture(nullptr), occlusionTexture(nullptr),
      emissiveTexture(nullptr), specularGlossinessTexture(nullptr), diffuseTexture(nullptr), uniformBuffer(nullptr),
      descriptorSet(nullptr)
{
}

Result Material::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    descriptorSet = context->allocateDescriptorSet();

    return Result::Continue;
}

Result Material::update(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uint32_t binding = 0;
    rhi::DescriptorInfoList descriptorInfoList;
    rhi::DescriptorList descriptorList;

    std::vector<uint32_t> offsets;
    if (uniformBuffer)
    {
        offsets.push_back(uniformBuffer->getOffset());
        auto bufferDescriptor = uniformBuffer->getBufferDescriptor();

        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic});
        descriptorList.push_back(
            {{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic}, bufferDescriptor});
    }

    if (baseColorTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  baseColorTexture->getImageDescriptor()});
    }

    if (metallicRoughnessTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  metallicRoughnessTexture->getImageDescriptor()});
    }

    if (normalTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  normalTexture->getImageDescriptor()});
    }

    if (occlusionTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  occlusionTexture->getImageDescriptor()});
    }

    /*
        if (emissiveTexture)
        {
            descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel,
       rhi::DescriptorType::Combined_Image_Sampler}); descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel,
       rhi::DescriptorType::Combined_Image_Sampler}, uniformBuffer->getImageDescriptor()});
        }
    */
    if (specularGlossinessTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  specularGlossinessTexture->getImageDescriptor()});
    }

    if (diffuseTexture)
    {
        descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler},
                                  diffuseTexture->getImageDescriptor()});
    }

    try(descriptorSet->init(context, descriptorInfoList));
    try(descriptorSet->update(context, descriptorList, offsets));

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
