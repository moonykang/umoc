#include "shader.h"
#include "common/hash.h"
#include "context.h"
#include "descriptor.h"
#include "platform/asset.h"
#include "platform/context.h"
#include "defines.h"

namespace rhi
{
VertexShaderBase* Context::allocateVertexShader(std::string name, VertexChannelFlags vertexChannelFlags)
{
    name.reserve(SHADER_KEY_SIZE);

    ResourceID id = Resource::generateID(name.data(), SHADER_KEY_SIZE);

    if (auto shader = shaderMap.find(id); shader != shaderMap.end())
    {
        return reinterpret_cast<VertexShaderBase*>(shader->second);
    }

    VertexShaderBase* vertexShader = createVertexShader(id, name, vertexChannelFlags);

    try_call(vertexShader->init(this));
    shaderMap.insert({vertexShader->getID(), vertexShader});

    return vertexShader;
}

PixelShaderBase* Context::allocatePixelShader(std::string name)
{
    name.reserve(SHADER_KEY_SIZE);

    ResourceID id = Resource::generateID(name.data(), SHADER_KEY_SIZE);

    if (auto shader = shaderMap.find(id); shader != shaderMap.end())
    {
        return reinterpret_cast<PixelShaderBase*>(shader->second);
    }

    PixelShaderBase* pixelShader = createPixelShader(id, name);

    try_call(pixelShader->init(this));
    shaderMap.insert({pixelShader->getID(), pixelShader});

    return pixelShader;
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

        try(initRHI(context));

        loaded = true;
    }
    return Result::Continue;
}

void ShaderBase::terminate(Context* context)
{
    terminateRHI(context);
}

size_t GraphicsPipelineState::getHash()
{
    pushConstantsHash = util::computeGenericHash(pushConstants.data(), sizeof(PushConstant) * pushConstants.size());
    return util::computeGenericHash(this, ((PipelineStateHashSize + 3) / 4) * 4);
}

} // namespace rhi
