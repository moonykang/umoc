#include "material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"

namespace model
{
namespace gltf
{

MaterialUniformBlock::MaterialUniformBlock()
    : alphaMode(AlphaMode::Opaque), materialFactors(glm::vec4(1.f)), baseColorFactor(glm::vec4(1.f))
{
}

Result Material::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    uniformBuffer = {context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo),
                     rhi::ShaderStage::Pixel}; // TODO

    return model::Material::init(platformContext);
}

Result Material::update(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    try(uniformBuffer.first->update(context, sizeof(MaterialUniformBlock), &ubo));

    return model::Material::update(platformContext);
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
} // namespace gltf
} // namespace model