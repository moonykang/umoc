#include "shader.h"
#include "common/hash.h"
#include "context.h"
#include "descriptor.h"
#include "platform/asset.h"
#include "platform/context.h"

namespace rhi
{

VertexShaderBase* Context::allocateVertexShader(std::string name, VertexChannelFlags vertexChannelFlags)
{
    VertexShaderBase* vertexShader = new VertexShaderBase(name, vertexChannelFlags);

    try_call(vertexShader->init(this));
    shaderMap.insert({vertexShader->getID(), vertexShader});

    return vertexShader;
}

PixelShaderBase* Context::allocatePixelShader(std::string name)
{
    PixelShaderBase* pixelShader = new PixelShaderBase(name);

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

        loaded = true;
    }
    return Result::Continue;
}

size_t GraphicsPipelineState::getHash()
{
    pushConstantsHash = util::computeGenericHash(pushConstants.data(), sizeof(PushConstant) * pushConstants.size());
    return util::computeGenericHash(this, ((PipelineStateHashSize + 3) / 4) * 4);
}

} // namespace rhi