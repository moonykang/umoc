#include "shader.h"
#include "common/hash.h"
#include "context.h"
#include "platform/asset.h"
#include "platform/context.h"

namespace rhi
{
Result ShaderBase::loadShader(Context* context)
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
} // namespace rhi