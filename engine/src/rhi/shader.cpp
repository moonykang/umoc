#include "shader.h"
#include "common/hash.h"
#include "context.h"
#include "descriptor.h"
#include "platform/asset.h"
#include "platform/context.h"

namespace rhi
{
void Context::registerShaderContainer(ShaderContainer* shaderContainer)
{
    shaderContainers.push_back(shaderContainer);
}

Result ShaderBase::init(Context* context)
{
    if (!loaded)
    {
        platform::Context* platformContext = reinterpret_cast<platform::Context*>(context);
        platform::Asset* assetManager = platformContext->getAssetManager();

        try(assetManager->loadShader(name, code));

        // Fill padding for hashing
        if (name.size() % 4 != 0)
        {
            uint32_t padding = 4 - (name.size() % 4);

            for (uint32_t i = 0; i < padding; i++)
            {
                name += ' ';
            }
        }

        hash = util::computeGenericHash(name.data(), name.size() * sizeof(char));

        loaded = true;
    }
    return Result::Continue;
}

size_t GraphicsPipelineState::getHash()
{
    return util::computeGenericHash(this, ((PipelineStateHashSize + 3) / 4) * 4);
}

ShaderContainer::ShaderContainer()
    : initialized(false), hash(0), vertexShader(nullptr), pixelShader(nullptr), descriptorSetLayouts()
{
}

Result ShaderContainer::init(Context* context)
{
    std::lock_guard<std::mutex> scopeLock(lock);
    if (initialized)
    {
        return Result::Continue;
    }

    if (vertexShader)
    {
        vertexShader->init(context);
    }

    if (pixelShader)
    {
        pixelShader->init(context);
    }

    std::vector<rhi::DescriptorInfoList> descriptorInfoListSets = getDescriptorListSet();

    for (auto& descriptorInfoList : descriptorInfoListSets)
    {
        DescriptorSetLayout* descriptorSetLayout = descriptorSetLayouts.emplace_back();
        descriptorSetLayout = context->allocateDescriptorSetLayout();
        descriptorSetLayout->init(context, descriptorInfoList);
    }

    initialized = true;
    context->registerShaderContainer(this);

    return Result::Continue;
};

void ShaderContainer::terminate(Context* context)
{
    for (auto descriptorSetLayout : descriptorSetLayouts)
    {
        TERMINATE(descriptorSetLayout, context);
    }
    descriptorSetLayouts.clear();
}

VertexShaderBase* ShaderContainer::getVertexShader()
{
    return vertexShader;
}

PixelShaderBase* ShaderContainer::getPixelShader()
{
    return pixelShader;
}
} // namespace rhi