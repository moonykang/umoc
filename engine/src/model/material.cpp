#include "material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/resources.h"
#include "rhi/shader.h"

namespace model
{
Material::Material()
    : baseColorTexture({nullptr, 0}), metallicRoughnessTexture({nullptr, 0}), normalTexture({nullptr, 0}),
      occlusionTexture({nullptr, 0}), emissiveTexture({nullptr, 0}), specularGlossinessTexture({nullptr, 0}),
      diffuseTexture({nullptr, 0}), uniformBuffer({nullptr, 0}), descriptorSet(nullptr)
{
}

Material::~Material()
{
    if (shaderParameters)
    {
        delete shaderParameters;
        shaderParameters = nullptr;
    }
}

Result Material::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    descriptorSet = context->allocateDescriptorSet();
    shaderParameters = new rhi::ShaderParameters();

    return Result::Continue;
}

Result Material::update(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uint32_t binding = 0;
    rhi::DescriptorInfoList descriptorInfoList;
    rhi::DescriptorList descriptorList;

    std::vector<uint32_t> offsets;
    if (uniformBuffer.first)
    {
        offsets.push_back(uniformBuffer.first->getOffset());
        descriptorInfoList.push_back({binding, uniformBuffer.second, rhi::DescriptorType::Uniform_Buffer_Dynamic});
        descriptorList.push_back({{binding++, uniformBuffer.second, rhi::DescriptorType::Uniform_Buffer_Dynamic},
                                  uniformBuffer.first->getBufferDescriptor()});
    }

    for (auto storageBuffer : externalStorageBuffers)
    {
        offsets.push_back(storageBuffer.first->getOffset());
        descriptorInfoList.push_back({binding, storageBuffer.second, rhi::DescriptorType::Storage_Buffer_Dynamic});
        descriptorList.push_back({{binding++, storageBuffer.second, rhi::DescriptorType::Storage_Buffer_Dynamic},
                                  storageBuffer.first->getBufferDescriptor()});
    }

    if (baseColorTexture.first)
    {
        descriptorInfoList.push_back({binding, baseColorTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, baseColorTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
                                  baseColorTexture.first->getImageDescriptor()});
    }

    if (metallicRoughnessTexture.first)
    {
        descriptorInfoList.push_back(
            {binding, metallicRoughnessTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back(
            {{binding++, metallicRoughnessTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
             metallicRoughnessTexture.first->getImageDescriptor()});
    }

    if (normalTexture.first)
    {
        descriptorInfoList.push_back({binding, normalTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, normalTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
                                  normalTexture.first->getImageDescriptor()});
    }

    if (occlusionTexture.first)
    {
        descriptorInfoList.push_back({binding, occlusionTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, occlusionTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
                                  occlusionTexture.first->getImageDescriptor()});
    }

    /*
        if (emissiveTexture)
        {
            descriptorInfoList.push_back({binding, rhi::ShaderStage::Pixel,
       rhi::DescriptorType::Combined_Image_Sampler}); descriptorList.push_back({{binding++, rhi::ShaderStage::Pixel,
       rhi::DescriptorType::Combined_Image_Sampler}, uniformBuffer->getImageDescriptor()});
        }
    */
    if (specularGlossinessTexture.first)
    {
        descriptorInfoList.push_back(
            {binding, specularGlossinessTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back(
            {{binding++, specularGlossinessTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
             specularGlossinessTexture.first->getImageDescriptor()});
    }

    if (diffuseTexture.first)
    {
        descriptorInfoList.push_back({binding, diffuseTexture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, diffuseTexture.second, rhi::DescriptorType::Combined_Image_Sampler},
                                  diffuseTexture.first->getImageDescriptor()});
    }

    for (auto texture : externalTextures)
    {
        descriptorInfoList.push_back({binding, texture.second, rhi::DescriptorType::Combined_Image_Sampler});
        descriptorList.push_back({{binding++, texture.second, rhi::DescriptorType::Combined_Image_Sampler},
                                  texture.first->getImageDescriptor()});
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

void Material::updateTexture(MaterialFlag materialFlag, rhi::Texture* texture, rhi::ShaderStageFlags shaderStageFlags)
{
    switch (materialFlag.get())
    {
    case MaterialFlag::BaseColorTexture:
        baseColorTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::MetalicRoughnessTexture:
        metallicRoughnessTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::NormalTexture:
        normalTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::OcclusionTexture:
        occlusionTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::EmissiveTexture:
        emissiveTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::SpecularGlossinessTexture:
        specularGlossinessTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::DiffuseTexture:
        diffuseTexture = {texture, shaderStageFlags};
        break;
    case MaterialFlag::External:
        externalTextures.push_back({texture, shaderStageFlags});
        break;
    }
}

void Material::updateStorageBuffer(rhi::SubAllocatedBuffer* storageBuffer, rhi::ShaderStageFlags shaderStageFlags)
{
    externalStorageBuffers.push_back({storageBuffer, shaderStageFlags});
}

void Material::updateUniformBuffer(rhi::SubAllocatedBuffer* uniformBuffer, rhi::ShaderStageFlags shaderStageFlags)
{
    this->uniformBuffer = {uniformBuffer, shaderStageFlags};
}

rhi::Texture* Material::getTexture(MaterialFlag materialFlag)
{
    switch (materialFlag.get())
    {
    case MaterialFlag::BaseColorTexture:
        return baseColorTexture.first;
    case MaterialFlag::MetalicRoughnessTexture:
        return metallicRoughnessTexture.first;
    case MaterialFlag::NormalTexture:
        return normalTexture.first;
    case MaterialFlag::OcclusionTexture:
        return occlusionTexture.first;
    case MaterialFlag::EmissiveTexture:
        return emissiveTexture.first;
    case MaterialFlag::SpecularGlossinessTexture:
        return specularGlossinessTexture.first;
    case MaterialFlag::DiffuseTexture:
        return diffuseTexture.first;
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

void Material::setShaderParameters(rhi::ShaderParameters* shaderParameters)
{
    this->shaderParameters = new rhi::ShaderParameters(*shaderParameters);
}
rhi::ShaderParameters* Material::getShaderParameters()
{
    ASSERT(shaderParameters);
    return shaderParameters;
}
} // namespace model
